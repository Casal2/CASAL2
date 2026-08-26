/**
 * @file RecruitmentBevertonHoltWithDeviations.cpp
 * @author C.Marsh
 * @date 11/7/17
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "RecruitmentBevertonHoltWithDeviations.h"

#include <limits>
#include <numeric>

#include "Categories/Categories.h"
#include "DerivedQuantities/Manager.h"
#include "Estimates/Manager.h"
#include "InitialisationPhases/Manager.h"
#include "TimeSteps/Manager.h"
#include "Utilities/Map.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"
#include "Utilities/Vector.h"

// namespaces
namespace niwa::processes::common {

namespace math = niwa::utilities::math;

/**
 * Default constructor
 */
RecruitmentBevertonHoltWithDeviations::RecruitmentBevertonHoltWithDeviations(shared_ptr<Model> model) : RecruitmentStockRecruit(model) {
  LOG_TRACE();

  // clang-format off
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The category labels")
    ->flag_is_category();
  parameters_.Bind<Double>(PARAM_R0, &r0_, "R0, the mean recruitment used to scale annual recruits or initialise the model")
    ->set_is_optional(true);
  parameters_.Bind<Double>(PARAM_B0, &b0_, "B0, the SSB corresponding to R0, and used to scale annual recruits or initialise the model")
    ->set_is_optional(true);
  parameters_.Bind<Double>(PARAM_PROPORTIONS, &proportions_, "The proportion for each category");
  parameters_.Bind<unsigned>(PARAM_AGE, &age_, "The age at recruitment")
    ->set_is_optional(true);
  parameters_.Bind<unsigned>(PARAM_SSB_OFFSET, &ssb_offset_, "The spawning biomass year offset")
    ->set_default_value(0u);
  parameters_.Bind<Double>(PARAM_STEEPNESS, &steepness_, "Steepness (h)")
    ->set_default_value(1.0);
  parameters_.Bind<string>(PARAM_SSB, &ssb_label_, "The SSB label (i.e., the derived quantity label))");
  parameters_.Bind<Double>(PARAM_SIGMA_R, &sigma_r_, "The standard deviation of recruitment, sigma_R");
  parameters_.Bind<Double>(PARAM_B_MAX, &b_max_, "The maximum bias adjustment")
    ->set_default_value(0.85);
  parameters_.Bind<unsigned>(PARAM_LAST_YEAR_WITH_NO_BIAS, &year1_, "The last year (recruited year) with no bias adjustment");
  parameters_.Bind<unsigned>(PARAM_FIRST_YEAR_WITH_BIAS, &year2_, "The first year (recruited year) with full bias adjustment");
  parameters_.Bind<unsigned>(PARAM_LAST_YEAR_WITH_BIAS, &year3_, "The last year (recruited year) with full bias adjustment");
  parameters_.Bind<unsigned>(PARAM_FIRST_RECENT_YEAR_WITH_NO_BIAS, &year4_, "The first recent year (recruited year) with no bias adjustment");
  parameters_.Bind<string>(PARAM_B0_PHASE, &phase_b0_label_, "The initialisation phase label that B0 is from")->set_default_value("");
  parameters_.Bind<Double>(PARAM_DEVIATION_VALUES, &recruit_dev_values_, "The recruitment deviation values");

  // deprecated parameters
  parameters_.Bind<unsigned>(PARAM_DEVIATION_YEARS, &recruit_dev_years_,"")->flag_deprecated();
  // clang-format on

  RegisterAsAddressable(PARAM_R0, &r0_);
  RegisterAsAddressable(PARAM_B0, &b0_);
  RegisterAsAddressable(PARAM_B_MAX, &b_max_);
  RegisterAsAddressable(PARAM_SIGMA_R, &sigma_r_);
  RegisterAsAddressable(PARAM_STEEPNESS, &steepness_);
  RegisterAsAddressable(PARAM_PROPORTIONS, &proportions_by_category_);
  RegisterAsAddressable(PARAM_DEVIATION_VALUES, &recruit_dev_value_by_year_);

  phase_b0_            = 0;
  process_type_        = ProcessType::kRecruitment;
  partition_structure_ = PartitionType::kAge;
}

/**
 * Validate
 */
void RecruitmentBevertonHoltWithDeviations::DoValidate() {
  LOG_TRACE();

  ValidateCategoriesProportionsAndCore();
  ValidateAndWarnAge();

  parameters_.Validate(PARAM_STEEPNESS)->GreaterThanOrEqualTo(0.2)->LessThanOrEqualTo(1.0);
  parameters_.Validate(PARAM_SIGMA_R)->GreaterThanOrEqualTo(0.0);
  parameters_.Validate(PARAM_B_MAX)->GreaterThanOrEqualTo(0.0)->LessThanOrEqualTo(1.0);
  parameters_.ValidateVector(PARAM_DEVIATION_VALUES)->NumberOfElements(years_.size());

  recruit_dev_value_by_year_ = utilities::Map::create(years_, recruit_dev_values_);
}

/**
 * Build the runtime relationships between this object and other objects
 */
void RecruitmentBevertonHoltWithDeviations::DoBuild() {
  ResolveB0AndDerivedQuantity();

  /**
   * Calculate out SSB offset
   */
  unsigned ageing_index, derived_quantity_index, recruitment_index, ageing_processes;
  ComputeTimeStepIndices(ageing_index, derived_quantity_index, recruitment_index, ageing_processes);

  if (ageing_processes > 1)
    LOG_ERROR_P(PARAM_SSB_OFFSET) << "The Beverton-Holt recruitment year offset has been calculated on the basis of a single ageing process. " << ageing_processes
                                  << " ageing processes were specified. Manually set the ssb_offset or contact the development team";
  if (ageing_index == std::numeric_limits<unsigned>::max())
    LOG_ERROR() << location() << " could not calculate the ssb_offset because there is no ageing process defined";

  unsigned temp_ssb_offset = 0;
  if (recruitment_index < ageing_index && ageing_index < derived_quantity_index)
    temp_ssb_offset = age_ + 1;
  else if (derived_quantity_index < ageing_index && ageing_index < recruitment_index)
    temp_ssb_offset = age_ - 1;
  else
    temp_ssb_offset = age_;

  LOG_FINEST() << "SSB offset calculated to be = " << temp_ssb_offset << "; recruitment index = " << recruitment_index << "; ageing index = " << ageing_index
               << "; derived_quantity index = " << derived_quantity_index;
  if (parameters_.Get(PARAM_SSB_OFFSET)->has_been_defined()) {
    // Check if the user has supplied the expected value for the model.
    if (temp_ssb_offset != ssb_offset_) {
      LOG_WARNING() << "The " << PARAM_SSB_OFFSET << " specified (" << ssb_offset_ << ") is different from what Casal2 calculated (" << temp_ssb_offset
                    << "). This value should be manually set only under certain conditions. See the User Manual on this process for more information.";
    }
  } else {
    ssb_offset_ = temp_ssb_offset;
  }

  BuildSpawnEventYears();
  ValidateR0B0NotBothEstimated();

  // Check if recruitment devs have an @estimate block, I am just checking over the first year
  for (auto year : years_) {
    string year_string;
    if (!utilities::To<unsigned, string>(year, year_string))
      LOG_CODE_ERROR() << "Could not convert the value " << year << " to a string";

    string recruit_parm = "process[" + label_ + "]." + PARAM_DEVIATION_VALUES + "{" + year_string + "}";
    if (model()->managers()->estimate()->HasEstimate(recruit_parm)) {
      Estimate* recruit_dev_estimate = model()->managers()->estimate()->GetEstimate(recruit_parm);
      if (!recruit_dev_estimate)
        LOG_CODE_ERROR() << "'!sigma_r_estimate_', parameter " << recruit_parm << " has estimate but the parameter cannot be estimated.";
      if (recruit_dev_estimate->type() != PARAM_NORMAL_BY_STDEV)
        LOG_ERROR() << "An @estimate block for " << recruit_parm << " is not of type " << PARAM_NORMAL_BY_STDEV << ". @estimate blocks only for deviation_values of type "
                    << PARAM_NORMAL_BY_STDEV << " are valid.";
      // Check sigma is the same as sigma_r
      map<string, Parameter*> parameters = recruit_dev_estimate->parameters().parameters();
      for (auto param = parameters.begin(); param != parameters.end(); ++param) {
        if (param->first == PARAM_SIGMA) {
          Double estimate_sigma = 0.0;
          if (!utilities::To<string, Double>(param->second->values()[0], estimate_sigma))
            LOG_CODE_ERROR() << "Could not convert the value " << param->second->values()[0] << " to a Double";

          if (fabs(estimate_sigma - sigma_r_) > 0.001) {
            LOG_FATAL_P(PARAM_SIGMA_R) << "@estimate block for parameter " << recruit_parm << " has sigma = " << estimate_sigma << ". sigma = " << sigma_r_
                                       << " is specified in this process. These two sigma values must be the same.";
          }
        }
      }
    }
  }

  // Build Bias correction map by year 'bias_by_year_'
  for (auto year : years_) {
    if (year <= year1_) {
      bias_by_year_[year] = 0.0;
    } else if ((year > year1_) && (year < year2_)) {
      bias_by_year_[year] = b_max_ * (Double)(1 - ((year - year1_) / (year2_ - year1_)));
    } else if ((year >= year2_) && (year <= year3_)) {
      bias_by_year_[year] = b_max_;
    } else if ((year > year3_) && (year < year4_)) {
      bias_by_year_[year] = b_max_ * (Double)(1 - ((year3_ - year) / (year4_ - year3_)));
    } else if (year >= year4_) {
      bias_by_year_[year] = 0.0;
    }
  }

  // Pre allocate memory for report objects
  ResizeYearlyCaches();
  ycs_values_.resize(model()->years().size());

  DoReset();
}

/**
 * Reset all of the values so they are ready for an execution run
 *
 * - check if is necessary to rescale the partition
 * - update input parameters to updated parameters
 * - clear reporting containers
 * - check where B0 is coming from
 */
void RecruitmentBevertonHoltWithDeviations::DoReset() {
  LOG_TRACE();

  // if a -i call is made then we need to re-populate the recruit_dev_values_ vector for reporting.
  // This has to be done because the input parameter recruit_dev_values_ and registered estimate parameter recruit_dev_value_by_year_
  // Are different
  for (unsigned i = 0; i < years_.size(); ++i) {
    recruit_dev_values_[i] = recruit_dev_value_by_year_[years_[i]];
  }

  ResetPartitionAndProportions();
  fill(ycs_values_.begin(), ycs_values_.end(), 0.0);

  // Check whether B0 as an input paramter or a derived quantity, this is a result of having an r0 or a b0 in the process
  // if its estimated or an input it will be updates.
  if (!parameters_.Get(PARAM_B0)->has_been_defined())
    b0_ = derived_quantity_->GetLastValueFromInitialisation(phase_b0_);

  // Only rebuild in the reset if Bmax is estimated, otherwise it remains constant.
  if (model()->managers()->estimate()->HasEstimate("process[" + label_ + "].b_max")) {  // this check should be moved into the Build and set a flag
    // Build Bias correction map by year 'bias_by_year_'
    for (auto year : recruit_dev_years_) {
      if (year <= year1_) {
        bias_by_year_[year] = 0.0;
      } else if ((year > year1_) && (year < year2_)) {
        bias_by_year_[year] = b_max_ * (Double)(1 - ((year - year1_) / (year2_ - year1_)));
      } else if ((year >= year2_) && (year <= year3_)) {
        bias_by_year_[year] = b_max_;
      } else if ((year > year3_) && (year < year4_)) {
        bias_by_year_[year] = b_max_ * (Double)(1 - ((year3_ - year) / (year4_ - year3_)));
      } else if (year >= year4_) {
        bias_by_year_[year] = 0.0;
      }
    }
  }
}

/**
 * The Beverton-Holt stock-recruit curve.
 */
Double RecruitmentBevertonHoltWithDeviations::CalculateSR(Double ssb_ratio) {
  return ssb_ratio / (1.0 - ((5.0 * steepness_ - 1.0) / (4.0 * steepness_)) * (1.0 - ssb_ratio));
}

/**
 * The YCS source: a bias-corrected lognormal deviation, rather than an input multiplier.
 */
Double RecruitmentBevertonHoltWithDeviations::CalculateYCS(unsigned current_year) {
  Double ycs = 0.0;
  if (recruit_dev_value_by_year_.find(current_year) != recruit_dev_value_by_year_.end() && bias_by_year_.find(current_year) != bias_by_year_.end())
    ycs = exp(recruit_dev_value_by_year_[current_year] - (bias_by_year_[current_year] * 0.5 * sigma_r_ * sigma_r_));
  LOG_FINEST() << "Projected ycs = " << ycs;

  if (ycs < 0.0)
    LOG_CODE_ERROR() << "'ycs < 0.0' negative recruitment";

  ycs_values_[year_counter_] = ycs;
  return ycs;
}

/**
 * Fill the report cache
 * @description A method for reporting process information
 * @param cache a cache object to print to
 */
void RecruitmentBevertonHoltWithDeviations::FillReportCache(ostringstream& cache) {
  cache << "recruitment_multipliers: ";
  for (auto iter : ycs_values_) cache << AS_DOUBLE(iter) << " ";
  cache << "\nmodel_year: ";
  for (auto iter : recruit_dev_value_by_year_) cache << iter.first << " ";
  AppendSpawnEventYearField(cache);
  AppendCoreReportFields(cache);
  cache << "\nrecruit_event_SSB_percent: ";
  for (auto iter : ssb_values_) cache << AS_DOUBLE(iter) / AS_DOUBLE(b0_) * 100.0 << " ";
  cache << REPORT_EOL;
}

/**
 * Fill the tabular report cache
 * @description A method for reporting tabular process information
 * @param cache a cache object to print to
 * @param first_run whether to print the header
 *
 */
void RecruitmentBevertonHoltWithDeviations::FillTabularReportCache(ostringstream& cache, bool first_run) {
  if (first_run) {
    vector<unsigned> years = model()->years();
    for (auto year : years) {
      unsigned ssb_year = year - ssb_offset_;
      cache << "ycs_values[" << ssb_year << "] ";
    }
    for (auto year : years) {
      unsigned ssb_year = year - ssb_offset_;
      cache << "true_ycs[" << ssb_year << "] ";
    }
    for (auto year : years) {
      cache << "Recruits[" << year << "] ";
    }
    for (auto year : years) {
      unsigned ssb_year = year - ssb_offset_;
      cache << "Recruit_event_SSB[" << ssb_year << "] ";
    }
    cache << "R0 B0 steepness ";
    cache << REPORT_EOL;
  }

  for (auto value : ycs_values_) cache << AS_DOUBLE(value) << " ";
  for (auto value : true_ycs_values_) cache << AS_DOUBLE(value) << " ";
  for (auto value : recruitment_values_) cache << AS_DOUBLE(value) << " ";
  for (auto value : ssb_values_) cache << AS_DOUBLE(value) << " ";
  cache << AS_DOUBLE(r0_) << " " << AS_DOUBLE(b0_) << " " << AS_DOUBLE(steepness_) << " ";
  cache << REPORT_EOL;
}

} /* namespace niwa::processes::common */
