/**
 * @file RecruitmentBevertonHolt.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 12/07/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * Consolidated Beverton-Holt recruitment implementation for both Age and
 * Length partitioned models.
 */

// headers
#include "RecruitmentBevertonHolt.h"

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
RecruitmentBevertonHolt::RecruitmentBevertonHolt(shared_ptr<Model> model) : RecruitmentStockRecruit(model) {
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
  parameters_.Bind<string>(PARAM_SSB, &ssb_label_, "The SSB label (i.e., the derived quantity label)");
  parameters_.Bind<string>(PARAM_B0_PHASE, &phase_b0_label_, "The initialisation phase label that B0 is from")
    ->set_is_optional(true);
  parameters_.Bind<Double>(PARAM_RECRUITMENT_MULTIPLIERS, &recruitment_multipliers_, "The YCS values");
  parameters_.Bind<unsigned>(PARAM_STANDARDISE_YEARS, &standardise_years_, "The years that are included for year class standardisation")
    ->set_is_optional(true);
  // Length-specific parameters
  parameters_.Bind<Double>(PARAM_INITIAL_MEAN_LENGTH, &initial_mean_length_, "Mean length at recruitment for each categories")
    ->set_is_optional(true);
  parameters_.Bind<Double>(PARAM_INITIAL_LENGTH_CV, &initial_length_cv_, "CV for recruitment of each categories")
    ->set_is_optional(true);
  // Deprecated parameters
  parameters_.Bind<unsigned>(PARAM_STANDARDISE_YCS_YEARS, &standardise_ycs_years_, "The years that are included for year class standardisation")
    ->flag_deprecated(PARAM_STANDARDISE_YEARS);
  parameters_.Bind<Double>(PARAM_YCS_VALUES, &ycs_values_, "The YCS values")
    ->flag_deprecated(PARAM_RECRUITMENT_MULTIPLIERS);
  parameters_.Bind<unsigned>(PARAM_YCS_YEARS, &ycs_years_, "The recruitment years. A vector of years that relates to the year of the spawning event that created this cohort")
    ->flag_deprecated();
  // clang-format on

  RegisterAsAddressable(PARAM_R0, &r0_);
  RegisterAsAddressable(PARAM_B0, &b0_);
  RegisterAsAddressable(PARAM_STEEPNESS, &steepness_);
  RegisterAsAddressable(PARAM_PROPORTIONS, &proportions_by_category_);
  RegisterAsAddressable(PARAM_RECRUITMENT_MULTIPLIERS, &recruitment_multipliers_by_year_);
  RegisterAsAddressable(PARAM_STANDARDISED_RECRUITMENT_MULTIPLIERS, &standardised_recruitment_multipliers_by_year_, addressable::kAll);

  phase_b0_            = 0;
  process_type_        = ProcessType::kRecruitment;
  partition_structure_ = PartitionType::kAge | PartitionType::kLength;
}

/**
 * Validate the process
 */
void RecruitmentBevertonHolt::DoValidate() {
  LOG_TRACE();

  ValidateCategoriesProportionsAndCore();

  parameters_.Validate(PARAM_STEEPNESS)->GreaterThanOrEqualTo(0.2)->LessThanOrEqualTo(1.0);
  parameters_.ValidateVector(PARAM_STANDARDISE_YEARS)->IsModelYear()->IsInIncreasingOrder()->DefaultToModelYearsOnly();

  if (process_profile_ == ProcessProfile::kAge) {
    ValidateAndWarnAge();
    parameters_.ValidateVector(PARAM_RECRUITMENT_MULTIPLIERS)->GreaterThanOrEqualTo(0.0)->ExpandToNumberOfElements(years_.size())->NumberOfElements(years_.size());
  } else {
    // Length-specific validation
    parameters_.ValidateVector(PARAM_RECRUITMENT_MULTIPLIERS)->GreaterThanOrEqualTo(0.0)->NumberOfElements(years_.size());
    parameters_.ValidateVector(PARAM_INITIAL_MEAN_LENGTH)->GreaterThan(0.0)->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)->SameNumberOfElementsAs(PARAM_CATEGORIES);
    parameters_.ValidateVector(PARAM_INITIAL_LENGTH_CV)->GreaterThan(0.0)->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)->SameNumberOfElementsAs(PARAM_CATEGORIES);

    // Register length-specific addressables
    RegisterAsAddressable(PARAM_INITIAL_MEAN_LENGTH, &initial_mean_length_);
    RegisterAsAddressable(PARAM_INITIAL_LENGTH_CV, &initial_length_cv_);
  }

  recruitment_multipliers_by_year_              = utilities::Map::create(years_, recruitment_multipliers_);
  standardised_recruitment_multipliers_by_year_ = utilities::Map::create(years_, recruitment_multipliers_);

  standardise_recruitment_multipliers_ = standardise_years_.size() != 0;
}

/**
 * Build the runtime relationships between this object and other objects
 */
void RecruitmentBevertonHolt::DoBuild() {
  ResolveB0AndDerivedQuantity();

  /**
   * Calculate out SSB offset (Age-specific complex logic)
   */
  if (process_profile_ == ProcessProfile::kAge) {
    unsigned ageing_index, derived_quantity_index, recruitment_index, ageing_processes;
    ComputeTimeStepIndices(ageing_index, derived_quantity_index, recruitment_index, ageing_processes);

    if ((ageing_processes > 1) & !parameters_.Get(PARAM_SSB_OFFSET)->has_been_defined()) {
      LOG_ERROR_P(PARAM_LABEL) << "For the Beverton-Holt recruitment process, " << PARAM_SSB_OFFSET
                               << " can only be derived when there is only one ageing process in the annual cycle. " << ageing_processes
                               << " ageing processes were specified. Manually set the " << PARAM_SSB_OFFSET;
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
  }

  BuildSpawnEventYears();
  ValidateR0B0NotBothEstimated();
  ResizeYearlyCaches();

  // Length-specific: calculate initial length distribution
  if (process_profile_ == ProcessProfile::kLength) {
    initial_length_distribution_.resize(category_labels_.size());
  }

  DoReset();
}

/**
 * Verify all of the values so they are ready for an execution run
 */
void RecruitmentBevertonHolt::DoVerify(shared_ptr<Model> model) {
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
  }

  if (model->run_mode() == RunMode::kProjection) {
    if (IsAddressableUsedFor(PARAM_STANDARDISED_RECRUITMENT_MULTIPLIERS, addressable::kProject) & IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kProject))
      LOG_ERROR_P(PARAM_LABEL) << "- found an @project for both " << PARAM_STANDARDISED_RECRUITMENT_MULTIPLIERS << " and " << PARAM_RECRUITMENT_MULTIPLIERS
                               << ". This is not allowed, you must choose one or the other, but not both.";
    if (IsAddressableUsedFor(PARAM_STANDARDISED_RECRUITMENT_MULTIPLIERS, addressable::kProject)) {
      LOG_FINE() << "Projecting standardised multipliers";
      project_standardised_ycs_ = true;
    } else if (IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kProject)) {
      LOG_FINE() << "Projecting unstandardised multipliers";
      project_standardised_ycs_ = false;
    } else {
      LOG_ERROR_P(PARAM_LABEL) << "- could not find an @project block for " << PARAM_RECRUITMENT_MULTIPLIERS << " or " << PARAM_STANDARDISED_RECRUITMENT_MULTIPLIERS;
    }
  }
}

/**
 * Reset all of the values so they are ready for an execution run
 */
void RecruitmentBevertonHolt::DoReset() {
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

  // Length-specific: calculate initial length distribution
  if (process_profile_ == ProcessProfile::kLength) {
    for (unsigned i = 0; i < category_labels_.size(); i++) {
      initial_length_distribution_[i] = utilities::math::distribution(model()->length_bins(), model()->length_plus(), Distribution::kNormal, initial_mean_length_[i],
                                                                      initial_mean_length_[i] * initial_length_cv_[i]);
      Double sum_temp                 = utilities::math::Sum(initial_length_distribution_[i]);
      if (initial_length_distribution_[i].empty()) {
        LOG_CODE_ERROR() << "Initial length distribution is empty for category " << category_labels_[i];
        continue;
      }
      // first bin is a min plus group
      initial_length_distribution_[i][0] += 1.0 - sum_temp;
    }
  }

  // if a -i call is made then we need to repopulate the ycs_values vector for reporting.
  for (unsigned i = 0; i < years_.size(); ++i) {
    recruitment_multipliers_[i]                              = recruitment_multipliers_by_year_[years_[i]];
    standardised_recruitment_multipliers_by_year_[years_[i]] = recruitment_multipliers_by_year_[years_[i]];
  }

  // Do Haist ycs Parametrisation
  if (standardise_recruitment_multipliers_) {
    mean_ycs_ = 0;
    for (unsigned i = 0; i < years_.size(); ++i) {
      for (unsigned j = 0; j < standardise_years_.size(); ++j) {
        if (years_[i] == standardise_years_[j]) {
          mean_ycs_ += recruitment_multipliers_by_year_[years_[i]];
          break;
        }
      }
    }

    mean_ycs_ /= standardise_years_.size();
    for (unsigned ycs_year : years_) {
      for (unsigned j = 0; j < standardise_years_.size(); ++j) {
        if (ycs_year == standardise_years_[j])
          standardised_recruitment_multipliers_by_year_[ycs_year] = recruitment_multipliers_by_year_[ycs_year] / mean_ycs_;
      }
    }
  } else {
    for (unsigned ycs_year : years_) {
      standardised_recruitment_multipliers_by_year_[ycs_year] = recruitment_multipliers_by_year_[ycs_year];
    }
  }
}

/**
 * The Beverton-Holt stock-recruit curve.
 */
Double RecruitmentBevertonHolt::CalculateSR(Double ssb_ratio) {
  return ssb_ratio / (1.0 - ((5.0 * steepness_ - 1.0) / (4.0 * steepness_)) * (1.0 - ssb_ratio));
}

/**
 * The YCS/recruitment-multiplier source for the current year, including projection handling.
 */
Double RecruitmentBevertonHolt::CalculateYCS(unsigned current_year) {
  unsigned ssb_year = current_year - ssb_offset_;
  LOG_FINEST() << "standardise_years_.size(): " << standardise_years_.size() << "; model_->current_year(): " << current_year
               << "; model_->start_year(): " << model()->start_year();
  Double ycs;
  // If projection mode ycs values get replaced with projected value from @project block
  if (model()->run_mode() == RunMode::kProjection) {
    if (project_standardised_ycs_) {
      if (standardised_recruitment_multipliers_by_year_[current_year] <= 0.0) {
        LOG_FATAL_P(PARAM_RECRUITMENT_MULTIPLIERS) << "Projection mode (-f) is being run but ycs values are = 0 for year " << model()->current_year()
                                                   << ", which will cause the recruitment process to supply 0 recruits. Please check the @project block for this parameter";
      }
    } else {
      if (recruitment_multipliers_by_year_[current_year] <= 0.0) {
        LOG_FATAL_P(PARAM_RECRUITMENT_MULTIPLIERS) << "Projection mode (-f) is being run but ycs values are = 0 for year " << model()->current_year()
                                                   << ", which will cause the recruitment process to supply 0 recruits. Please check the @project block for this parameter";
      }
    }
    ycs = recruitment_multipliers_by_year_[current_year];
    if (current_year > model()->final_year()) {
      if (project_standardised_ycs_) {
        recruitment_multipliers_by_year_[current_year] = standardised_recruitment_multipliers_by_year_[current_year];
        ycs                                            = standardised_recruitment_multipliers_by_year_[current_year];
      } else {
        standardised_recruitment_multipliers_by_year_[current_year] = recruitment_multipliers_by_year_[current_year];
      }
    } else {
      if (ycs != recruitment_multipliers_[year_counter_])
        standardised_recruitment_multipliers_by_year_[current_year] = ycs;
      else
        ycs = standardised_recruitment_multipliers_by_year_[current_year];
    }
    LOG_FINE() << "ssb year = " << ssb_year << " value = " << ycs << " last val = " << model()->final_year() << " counter = " << year_counter_ << " size of vector "
               << recruitment_multipliers_.size();
    LOG_FINE() << "Projected ycs = " << ycs << " what is in the original " << recruitment_multipliers_[year_counter_];
  } else {
    ycs = standardised_recruitment_multipliers_by_year_[current_year];
    LOG_FINE() << "ycs" << ycs << " for SSB year " << ssb_year << " current year = " << current_year;
  }
  return ycs;
}

/**
 * Distribute recruits to the partition: Age puts recruits into the single age bin (inherited
 * default); Length distributes across length bins via the initial length distribution.
 */
void RecruitmentBevertonHolt::DistributeRecruits(Double amount_per) {
  if (process_profile_ == ProcessProfile::kAge) {
    RecruitmentStockRecruit::DistributeRecruits(amount_per);
    return;
  }

  // Length-specific: distribute across length bins
  unsigned category_counter = 0;
  for (auto category : partition_) {
    LOG_FINEST() << category->name_ << " recruits = " << amount_per << ", proportion of recruits " << proportions_by_category_[category->name_];
    unsigned limit = std::min<unsigned>(category->data_.size(), initial_length_distribution_[category_counter].size());
    if (limit != category->data_.size()) {
      LOG_CODE_ERROR() << "Length distribution size mismatch for category " << category->name_ << ": category bins = " << category->data_.size()
                       << ", distribution bins = " << initial_length_distribution_[category_counter].size();
    }
    for (unsigned i = 0; i < limit; i++) category->data_[i] += amount_per * initial_length_distribution_[category_counter][i] * proportions_by_category_[category->name_];
    ++category_counter;
  }
}

/**
 * Fill the report cache
 */
void RecruitmentBevertonHolt::FillReportCache(ostringstream& cache) {
  cache << "model_year: ";
  for (auto iter : standardised_recruitment_multipliers_by_year_) cache << iter.first << " ";
  AppendSpawnEventYearField(cache);
  cache << "\nstandardised_recruitment_multipliers: ";
  for (auto iter : standardised_recruitment_multipliers_by_year_) cache << AS_DOUBLE(iter.second) << " ";
  cache << "\nrecruitment_multipliers: ";
  for (auto iter : recruitment_multipliers_by_year_) cache << AS_DOUBLE(iter.second) << " ";
  AppendCoreReportFields(cache);
  cache << "\nssb_offset: " << ssb_offset_;
  cache << "\nrecruit_event_SSB_percent: ";
  for (auto iter : ssb_values_) cache << AS_DOUBLE(iter) / AS_DOUBLE(b0_) * 100.0 << " ";
  cache << REPORT_EOL;

  // Length-specific: output initial length distribution
  if (process_profile_ == ProcessProfile::kLength) {
    for (unsigned i = 0; i < category_labels_.size(); i++) {
      cache << category_labels_[i] << "_initial_length_distribution:";
      for (unsigned j = 0; j < initial_length_distribution_[i].size(); j++) cache << " " << AS_DOUBLE(initial_length_distribution_[i][j]);
      cache << REPORT_EOL;
    }
  }
}

/**
 * Fill the tabular report cache
 */
void RecruitmentBevertonHolt::FillTabularReportCache(ostringstream& cache, bool first_run) {
  FillMultiplierTabularReportCache(cache, first_run, standardised_recruitment_multipliers_by_year_, recruitment_multipliers_by_year_);
}

}  // namespace niwa::processes::common
