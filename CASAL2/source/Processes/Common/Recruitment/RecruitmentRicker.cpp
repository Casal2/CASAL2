/**
 * @file RecruitmentRicker.cpp
 * @author  A Dunn
 * @date June 2023
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "RecruitmentRicker.h"

#include <limits>
#include <numeric>

#include "AddressableTransformations/Common/Simplex.h"
#include "AddressableTransformations/Manager.h"
#include "Categories/Categories.h"
#include "DerivedQuantities/Manager.h"
#include "Estimates/Manager.h"
#include "InitialisationPhases/Manager.h"
#include "Model/Managers.h"
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
RecruitmentRicker::RecruitmentRicker(shared_ptr<Model> model) : RecruitmentStockRecruit(model) {
  LOG_TRACE();

  // clang-format off
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The category labels")->flag_is_category();
  parameters_.Bind<Double>(PARAM_R0, &r0_, "R0, the mean recruitment used to scale annual recruits or initialise the model")
    ->set_is_optional(true);
  parameters_.Bind<Double>(PARAM_B0, &b0_, "B0, the SSB corresponding to R0, and used to scale annual recruits or initialise the model")
    ->set_is_optional(true);
  parameters_.Bind<Double>(PARAM_PROPORTIONS, &proportions_, "The proportion for each category");
  parameters_.Bind<unsigned>(PARAM_AGE, &age_, "The age at recruitment")
    ->set_is_optional(true);
  parameters_.Bind<unsigned>(PARAM_SSB_OFFSET, &ssb_offset_, "The spawning biomass year offset")
    ->set_default_value(0u);
  parameters_.Bind<Double>(PARAM_STEEPNESS, &steepness_, "Steepness (h)", "", 1.0)->set_range(0.2, 1.0);
  parameters_.Bind<string>(PARAM_SSB, &ssb_label_, "The SSB label (i.e., the derived quantity label)", "");
  parameters_.Bind<string>(PARAM_B0_PHASE, &phase_b0_label_, "The initialisation phase label that B0 is from", "", "");

  parameters_.Bind<Double>(PARAM_RECRUITMENT_MULTIPLIERS, &recruitment_multipliers_, "The YCS values", "");
  parameters_.Bind<unsigned>(PARAM_STANDARDISE_YEARS, &standardise_years_, "The years that are included for year class standardisation", "", true);

  // these inputs are deprecated
  parameters_.Bind<unsigned>(PARAM_STANDARDISE_YCS_YEARS, &standardise_ycs_years_, "")->flag_deprecated(PARAM_STANDARDISE_YEARS);
  parameters_.Bind<Double>(PARAM_YCS_VALUES, &ycs_values_, "")->flag_deprecated(PARAM_RECRUITMENT_MULTIPLIERS);
  parameters_.Bind<unsigned>(PARAM_YCS_YEARS, &ycs_years_, "")->flag_deprecated();
  // clang-format on

  RegisterAsAddressable(PARAM_R0, &r0_);
  RegisterAsAddressable(PARAM_B0, &b0_);
  RegisterAsAddressable(PARAM_STEEPNESS, &steepness_);
  RegisterAsAddressable(PARAM_PROPORTIONS, &proportions_by_category_);
  RegisterAsAddressable(PARAM_RECRUITMENT_MULTIPLIERS, &recruitment_multipliers_by_year_);

  // Allow these to be used in additional priors.
  RegisterAsAddressable(PARAM_STANDARDISED_RECRUITMENT_MULTIPLIERS, &standardised_recruitment_multipliers_by_year_, addressable::kLookup);

  phase_b0_            = 0;
  process_type_        = ProcessType::kRecruitment;
  partition_structure_ = PartitionType::kAge;
}

/**
 * Validate the process
 */
void RecruitmentRicker::DoValidate() {
  LOG_TRACE();

  ValidateCategoriesProportionsAndCore();
  ValidateAndWarnAge();
  parameters_.ValidateVector(PARAM_RECRUITMENT_MULTIPLIERS)->GreaterThanOrEqualTo(0.0)->NumberOfElements(years_.size());
  parameters_.ValidateVector(PARAM_STANDARDISE_YEARS)->IsModelYear()->IsInIncreasingOrder();

  recruitment_multipliers_by_year_              = utilities::Map::create(years_, recruitment_multipliers_);
  standardised_recruitment_multipliers_by_year_ = utilities::Map::create(years_, recruitment_multipliers_);
}

/**
 * Build the runtime relationships between this object and other objects
 */
void RecruitmentRicker::DoBuild() {
  ResolveB0AndDerivedQuantity();

  /**
   * Calculate out SSB offset
   */
  unsigned ageing_index, derived_quantity_index, recruitment_index, ageing_processes;
  ComputeTimeStepIndices(ageing_index, derived_quantity_index, recruitment_index, ageing_processes);

  if ((ageing_processes > 1) & !parameters_.Get(PARAM_SSB_OFFSET)->has_been_defined()) {
    LOG_ERROR_P(PARAM_LABEL) << "For the Ricker recruitment process, " << PARAM_SSB_OFFSET << " can only be derived when there is only one ageing process in the annual cycle. "
                             << ageing_processes << " ageing processes were specified. Manually set the " << PARAM_SSB_OFFSET;
  }

  if (ageing_processes == 1) {
    if (ageing_index == std::numeric_limits<unsigned>::max())
      LOG_ERROR() << location() << " could not calculate the " << PARAM_SSB_OFFSET << " because there is no ageing process";

    unsigned temp_ssb_offset = 0;
    if ((recruitment_index < ageing_index) && (ageing_index < derived_quantity_index))
      temp_ssb_offset = age_ + 1;
    else if (derived_quantity_index < ageing_index && ageing_index < recruitment_index)
      temp_ssb_offset = age_ - 1;
    else
      temp_ssb_offset = age_;

    LOG_FINEST() << PARAM_SSB_OFFSET << " calculated to be = " << temp_ssb_offset << "; recruitment index = " << recruitment_index << "; ageing index = " << ageing_index
                 << "; derived_quantity index = " << derived_quantity_index;
    // Check if the user has supplied the expected value for the model.
    if (parameters_.Get(PARAM_SSB_OFFSET)->has_been_defined()) {
      if (temp_ssb_offset != ssb_offset_) {
        LOG_WARNING() << "The " << PARAM_SSB_OFFSET << " specified (" << ssb_offset_ << ") is different from what Casal2 calculated (" << temp_ssb_offset
                      << "). This value should be manually set only under certain conditions. See the User Manual on this process for more information.";
      }
    } else {
      ssb_offset_ = temp_ssb_offset;
    }
  }

  BuildSpawnEventYears();
  ValidateR0B0NotBothEstimated();
  ResizeYearlyCaches();

  DoReset();
}

/**
 * Verify all of the values so they are ready for an execution run
 */
void RecruitmentRicker::DoVerify(shared_ptr<Model> model) {
  // Check if PARAM_RECRUITMENT_MULTIPLIERS is used
  LOG_FINE() << "check transform usage = " << IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kTransformation);
  LOG_FINE() << "check lookup usage = " << IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kLookup);

  if (IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kTransformation)) {
    if (parameters_.Get(PARAM_STANDARDISE_YEARS)->has_been_defined()) {
      LOG_ERROR_P(PARAM_STANDARDISE_YEARS) << "When a @parameter_transformation of type simplex is applied to " << PARAM_RECRUITMENT_MULTIPLIERS << ", " << PARAM_STANDARDISE_YEARS
                                           << " must not be specified - it is auto-detected from the simplex year range. "
                                           << "Remove the " << PARAM_STANDARDISE_YEARS << " subcommand.";
    } else if (standardise_recruitment_multipliers_) {
      // Detection already performed in DoReset(); report the detected range
      LOG_INFO() << PARAM_STANDARDISE_YEARS << " defaulting to " << standardise_years_.front() << ":" << standardise_years_.back() << " (" << standardise_years_.size()
                 << " years)";
    }
  } else if (!parameters_.Get(PARAM_STANDARDISE_YEARS)->has_been_defined()) {
    // No transformation and no user-supplied standardise_years - disable standardisation
    standardise_years_.clear();
    standardise_recruitment_multipliers_ = false;
  }
}

/**
 * Reset all of the values so they are ready for an execution run
 */
void RecruitmentRicker::DoReset() {
  LOG_TRACE();

  // Auto-detect simplex standardise_years_ on first reset (DoReset runs inside Build(), before DoVerify)
  if (!simplex_standardise_detected_) {
    simplex_standardise_detected_ = true;
    if (IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kTransformation) && !parameters_.Get(PARAM_STANDARDISE_YEARS)->has_been_defined()) {
      bool   simplex_found   = false;
      string expected_prefix = "process[" + label_ + "]." + PARAM_RECRUITMENT_MULTIPLIERS;
      for (auto* trans : model()->managers()->addressable_transformation()->objects()) {
        auto* simplex_trans = dynamic_cast<niwa::addressabletransformations::Simplex*>(trans);
        if (!simplex_trans)
          continue;
        for (const auto& param_label : simplex_trans->GetParameterLabels()) {
          if (param_label.find(expected_prefix) == 0) {
            auto indices = simplex_trans->GetMapIndices(0);
            if (!indices.empty()) {
              standardise_years_.assign(indices.begin(), indices.end());
              std::sort(standardise_years_.begin(), standardise_years_.end());
              simplex_found = true;
            }
            break;
          }
        }
      }
      standardise_recruitment_multipliers_ = simplex_found;
      if (!simplex_found)
        standardise_years_.clear();
    }
  }

  ResetPartitionAndProportions();

  // if a -i call is made then we need to repopulate the ycs_values vector for reporting.
  // This has to be done because the input parameter ycs_values and registered estimate parameter ycs_values_by_year
  // Are different
  for (unsigned i = 0; i < years_.size(); ++i) {
    recruitment_multipliers_[i]                              = recruitment_multipliers_by_year_[years_[i]];
    standardised_recruitment_multipliers_by_year_[years_[i]] = recruitment_multipliers_by_year_[years_[i]];
  }

  // Do Haist ycs Parametrisation
  if (standardise_recruitment_multipliers_) {
    Double mean_ycs = 0;
    for (unsigned i = 0; i < years_.size(); ++i) {
      for (unsigned j = 0; j < standardise_years_.size(); ++j) {
        if (years_[i] == standardise_years_[j]) {
          mean_ycs += recruitment_multipliers_by_year_[years_[i]];
          break;
        }
      }
    }

    mean_ycs /= standardise_years_.size();
    for (unsigned ycs_year : years_) {
      for (unsigned j = 0; j < standardise_years_.size(); ++j) {
        if (ycs_year == standardise_years_[j])
          standardised_recruitment_multipliers_by_year_[ycs_year] = recruitment_multipliers_by_year_[ycs_year] / mean_ycs;
      }
    }
  } else {
    for (unsigned ycs_year : years_) {
      standardised_recruitment_multipliers_by_year_[ycs_year] = recruitment_multipliers_by_year_[ycs_year];
    }
  }
}

/**
 * The Ricker stock-recruit curve.
 */
Double RecruitmentRicker::CalculateSR(Double ssb_ratio) {
  return ssb_ratio * pow((1 / (5 * steepness_)), (1.25 * (ssb_ratio - 1)));
}

/**
 * The YCS/recruitment-multiplier source for the current year, including projection handling.
 */
Double RecruitmentRicker::CalculateYCS(unsigned current_year) {
  unsigned ssb_year = current_year - ssb_offset_;
  LOG_FINEST() << "standardise_years_.size(): " << standardise_years_.size() << "; model_->current_year(): " << current_year
               << "; model_->start_year(): " << model()->start_year();
  Double ycs;
  // If projection mode ycs values get replaced with projected value from @project block
  // note that the container recruitment_multipliers_by_year_ is changed by time_varying and projection classes
  // but the code wants to use standardised_recruitment_multipliers_by_year_ in the functions following here, so we might need to update this.
  if (model()->run_mode() == RunMode::kProjection) {
    if (recruitment_multipliers_by_year_[current_year] == 0.0) {
      LOG_FATAL_P(PARAM_RECRUITMENT_MULTIPLIERS) << "Projection mode (-f) is being run but ycs values are = 0 for year " << model()->current_year()
                                                 << ", which will cause the recruitment process to supply 0 recruits. Please check the @project block for this parameter";
    }
    // Projection classes will update this container automatically
    ycs = recruitment_multipliers_by_year_[current_year];
    // We need to see if this value has changed from the initial input, if it has we are going to assume that this is because the projection class has changed it.
    // set standardised ycs = ycs for reporting
    LOG_FINE() << "ssb year = " << ssb_year << " value = " << ycs << " last val = " << model()->final_year() << " counter = " << year_counter_ << " size of vector "
               << recruitment_multipliers_.size();
    if (current_year > model()->final_year()) {
      // we are in projection years so force standardised ycs to be the same as recruitment_multipliers_by_year_[ssb_year];
      standardised_recruitment_multipliers_by_year_[current_year] = ycs;
    } else {
      // we are still within start-final_year need to check if we have overwritten any values.
      if (ycs != recruitment_multipliers_[year_counter_])
        standardised_recruitment_multipliers_by_year_[current_year] = ycs;
      else
        ycs = standardised_recruitment_multipliers_by_year_[current_year];
    }
    LOG_FINE() << "Projected ycs = " << ycs << " what is in the original " << recruitment_multipliers_[year_counter_];
    // else business as usual
  } else {
    ycs = standardised_recruitment_multipliers_by_year_[current_year];
    LOG_FINE() << "ycs" << ycs << " for SSB year " << ssb_year << " current year = " << current_year;
  }
  return ycs;
}

/**
 * Fill the report cache
 */
void RecruitmentRicker::FillReportCache(ostringstream& cache) {
  cache << "model_year: ";
  for (auto iter : standardised_recruitment_multipliers_by_year_) cache << iter.first << " ";
  AppendSpawnEventYearField(cache);
  cache << "\nstandardised_recruitment_multipliers: ";
  for (auto iter : standardised_recruitment_multipliers_by_year_) cache << AS_DOUBLE(iter.second) << " ";
  cache << "\nrecruitment_multipliers: ";
  for (auto iter : recruitment_multipliers_by_year_) cache << AS_DOUBLE(iter.second) << " ";
  AppendCoreReportFields(cache);
  cache << "\nrecruit_event_SSB_percent: ";
  for (auto iter : ssb_values_) cache << AS_DOUBLE(iter) / AS_DOUBLE(b0_) * 100.0 << " ";
  cache << "\nssb_offset: " << ssb_offset_;
  cache << REPORT_EOL;
}

/**
 * Fill the tabular report cache
 */
void RecruitmentRicker::FillTabularReportCache(ostringstream& cache, bool first_run) {
  FillMultiplierTabularReportCache(cache, first_run, standardised_recruitment_multipliers_by_year_, recruitment_multipliers_by_year_);
}

} /* namespace niwa::processes::common */
