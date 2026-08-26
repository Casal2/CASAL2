/**
 * @file RecruitmentStockRecruit.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 26/08/2026
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "RecruitmentStockRecruit.h"

#include <limits>

#include "DerivedQuantities/Manager.h"
#include "InitialisationPhases/Manager.h"
#include "Model/Managers.h"
#include "TimeSteps/Manager.h"
#include "Utilities/To.h"
#include "Utilities/Vector.h"

// namespaces
namespace niwa::processes::common {

/**
 * Default constructor
 */
RecruitmentStockRecruit::RecruitmentStockRecruit(shared_ptr<Model> model) : Process(model), partition_(model) {
  LOG_TRACE();
}

/**
 * Shared validation: R0/B0 either-or, proportions, ssb_offset, proportions_by_category_ map.
 * Steepness validation is left to each subclass because the binding mechanism differs
 * (Ricker validates via a constructor-bound range, the others via an explicit Validate() call).
 */
void RecruitmentStockRecruit::ValidateCategoriesProportionsAndCore() {
  for (auto year = model()->start_year(); year <= model()->final_year(); ++year) years_.push_back(year);

  parameters_.Validate(PARAM_R0)->GreaterThanOrEqualTo(0.0)->EitherOrDefined(PARAM_B0);
  parameters_.Validate(PARAM_B0)->GreaterThanOrEqualTo(0.0)->ForbiddenIfDefined(PARAM_R0);
  parameters_.ValidateVector(PARAM_PROPORTIONS)
      ->GreaterThanOrEqualTo(0.0)
      ->LessThanOrEqualTo(1.0)
      ->SumToOne()
      ->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)
      ->SameNumberOfElementsAs(PARAM_CATEGORIES);
  parameters_.Validate(PARAM_SSB_OFFSET)->GreaterThanOrEqualTo(0u)->LessThanOrEqualTo(model()->final_year() - model()->start_year());

  proportions_by_category_ = utilities::OrderedMap<string, Double>::create(category_labels_, proportions_);
}

/**
 * Shared "age" parameter validation + the recruitment-age-vs-model-min-age warning.
 */
void RecruitmentStockRecruit::ValidateAndWarnAge() {
  parameters_.Validate(PARAM_AGE)->IsAge()->DefaultValue(model()->min_age());
  if (age_ != model()->min_age()) {
    LOG_WARNING_P(PARAM_AGE) << "(" << age_ << ") is not equal to the model min_age (" << model()->min_age()
                             << "). This is likely an error. Please check your input configuration files";
  }
}

/**
 * Shared DoBuild opening block: initialise the partition accessor, resolve b0_initialised_,
 * resolve phase_b0_, and resolve derived_quantity_.
 */
void RecruitmentStockRecruit::ResolveB0AndDerivedQuantity() {
  partition_.Init(category_labels_);

  if (parameters_.Get(PARAM_B0)->has_been_defined()) {
    model()->set_b0_initialised(ssb_label_, true);
    b0_initialised_ = true;
  }
  if (phase_b0_label_ != "")
    phase_b0_ = model()->managers()->initialisation_phase()->GetPhaseIndex(phase_b0_label_);

  derived_quantity_ = model()->managers()->derived_quantity()->GetDerivedQuantity(ssb_label_);
  if (!derived_quantity_)
    LOG_ERROR_P(PARAM_SSB) << "Derived quantity SSB (" << ssb_label_ << ") was not found.";
}

/**
 * Walk the ordered time steps to locate the ageing, mortality/derived-quantity, and
 * recruitment process indices used to auto-derive ssb_offset_. Identical across all three
 * stock-recruit children; each child applies its own guard/error logic to the results
 * because that logic genuinely differs between them.
 */
void RecruitmentStockRecruit::ComputeTimeStepIndices(unsigned& ageing_index, unsigned& derived_quantity_index, unsigned& recruitment_index, unsigned& ageing_processes) {
  ageing_index            = std::numeric_limits<unsigned>::max();
  derived_quantity_index  = std::numeric_limits<unsigned>::max();
  recruitment_index       = std::numeric_limits<unsigned>::max();
  ageing_processes        = 0;

  const vector<TimeStep*> ordered_time_steps               = model()->managers()->time_step()->ordered_time_steps();
  unsigned                time_step_index                  = 0;
  unsigned                process_index                    = 0;
  unsigned                derived_quantity_time_step_index = model()->managers()->time_step()->GetTimeStepIndex(derived_quantity_->time_step());
  bool                    mortality_block                  = false;

  // loop through time steps
  for (auto time_step : ordered_time_steps) {
    if (time_step_index == derived_quantity_time_step_index) {
      for (auto process : time_step->processes()) {
        if (process->process_type() == ProcessType::kAgeing) {
          ageing_index = process_index;
          ageing_processes++;
        }
        if (process->process_type() == ProcessType::kMortality) {
          mortality_block        = true;
          derived_quantity_index = process_index;
        }
        process_index++;
      }
      LOG_FINEST() << "process_index = " << process_index;
      if (!mortality_block) {
        process_index++;
        derived_quantity_index = process_index;
        process_index++;
      }
    } else {
      for (auto process : time_step->processes()) {
        if (process->process_type() == ProcessType::kAgeing) {
          ageing_index = process_index;
          ageing_processes++;
        }
        process_index++;
      }
    }
    time_step_index++;
  }

  recruitment_index = model()->managers()->time_step()->GetProcessIndex(label_);
}

/**
 * Resize and fill spawn_event_years_ from ssb_offset_.
 */
void RecruitmentStockRecruit::BuildSpawnEventYears() {
  spawn_event_years_.resize(model()->years().size(), 0.0);
  for (unsigned year_iter = 0; year_iter < model()->years().size(); ++year_iter) {
    spawn_event_years_[year_iter] = model()->years()[year_iter] - ssb_offset_;
    LOG_FINEST() << "ssb year = " << spawn_event_years_[year_iter] << " for year = " << model()->years()[year_iter];
  }
}

/**
 * Check the user hasn't specified an @estimate block for both R0 and B0.
 */
void RecruitmentStockRecruit::ValidateR0B0NotBothEstimated() {
  if (IsAddressableUsedFor(PARAM_R0, addressable::kEstimate) & IsAddressableUsedFor(PARAM_B0, addressable::kEstimate))
    LOG_ERROR() << "Both R0 and B0 have an @estimate specified for recruitment process " << label_ << ". Only one of these parameters can be estimated.";
}

/**
 * Pre-allocate the per-year reporting caches.
 */
void RecruitmentStockRecruit::ResizeYearlyCaches() {
  ssb_values_.resize(model()->years().size());
  true_ycs_values_.resize(model()->years().size());
  recruitment_values_.resize(model()->years().size());
}

/**
 * Shared DoReset fragment: reset have_scaled_partition, repopulate proportions_ from the
 * addressable map, and zero the per-year reporting caches.
 */
void RecruitmentStockRecruit::ResetPartitionAndProportions() {
  if (parameters_.Get(PARAM_B0)->has_been_defined()) {
    have_scaled_partition = false;
  }

  unsigned iter = 0;
  for (auto& category : category_labels_) {
    proportions_[iter] = proportions_by_category_[category];
    ++iter;
  }

  fill(ssb_values_.begin(), ssb_values_.end(), 0.0);
  fill(true_ycs_values_.begin(), true_ycs_values_.end(), 0.0);
  fill(recruitment_values_.begin(), recruitment_values_.end(), 0.0);
}

/**
 * Execute this process. Identical control flow across all three stock-recruit children once
 * the SR formula (CalculateSR) and YCS source (CalculateYCS) are factored out as hooks.
 */
void RecruitmentStockRecruit::DoExecute() {
  unsigned             current_year   = model()->current_year();
  std::pair<bool, int> this_year_iter = niwa::utilities::findInVector(model()->years(), current_year);
  year_counter_                       = this_year_iter.second;
  unsigned ssb_year                   = current_year - ssb_offset_;
  LOG_FINE() << "year = " << current_year << " ssb year = " << ssb_year << " year counter = " << year_counter_;

  Double amount_per = 0.0;
  if (model()->state() == State::kInitialise) {
    initialisationphases::Manager& init_phase_manager = *model()->managers()->initialisation_phase();
    if ((init_phase_manager.last_executed_phase() <= phase_b0_) && (parameters_.Get(PARAM_R0)->has_been_defined())) {
      amount_per = r0_;
    } else if ((init_phase_manager.last_executed_phase() <= phase_b0_) && (parameters_.Get(PARAM_B0)->has_been_defined())) {
      if (have_scaled_partition)
        amount_per = r0_;
      else
        amount_per = 1;
    } else {
      // if R0 initialised mode then b0 is a derived quantity
      if (!parameters_.Get(PARAM_B0)->has_been_defined())
        b0_ = derived_quantity_->GetLastValueFromInitialisation(phase_b0_);

      Double SSB       = derived_quantity_->GetLastValueFromInitialisation(init_phase_manager.last_executed_phase());
      Double ssb_ratio = SSB / b0_;
      Double true_ycs  = 1.0 * CalculateSR(ssb_ratio);
      amount_per       = r0_ * true_ycs;

      LOG_FINE() << "B0_: " << b0_ << "; ssb_ratio: " << ssb_ratio << "; true_ycs: " << true_ycs << "; amount_per: " << amount_per << " R0 = " << r0_;
    }
    LOG_FINE() << "Initialise: amount_per = " << amount_per;
  } else {
    /**
     * The model is not in an initialisation phase
     */
    Double ycs = CalculateYCS(current_year);

    // Check whether B0 as an input parameter or a derived quantity
    if (!parameters_.Get(PARAM_B0)->has_been_defined())
      b0_ = derived_quantity_->GetLastValueFromInitialisation(phase_b0_);

    // Calculate year to get SSB that contributes to this years recruits
    Double SSB;
    if (ssb_year < model()->start_year()) {
      // Model is in normal years but requires an SSB from the initialisation phase
      initialisationphases::Manager& init_phase_manager = *model()->managers()->initialisation_phase();
      LOG_FINE() << "Initialisation phase index SSB is being extracted from init phase " << init_phase_manager.last_executed_phase() << " SSB year = " << ssb_year;
      SSB = derived_quantity_->GetLastValueFromInitialisation(init_phase_manager.last_executed_phase());
    } else {
      SSB = derived_quantity_->GetValue(ssb_year);
    }

    Double ssb_ratio = SSB / b0_;
    Double SR        = CalculateSR(ssb_ratio);
    Double true_ycs  = ycs * SR;
    amount_per       = r0_ * true_ycs;

    true_ycs_values_[year_counter_]    = true_ycs;
    recruitment_values_[year_counter_] = amount_per;
    ssb_values_[year_counter_]         = SSB;

    LOG_FINEST() << "year = " << model()->current_year() << " SSB = " << SSB << " SR = " << SR << "; ycs = " << ycs << "; B0_ = " << b0_ << "; R0 = " << r0_
                 << "; ssb_ratio = " << ssb_ratio << "; true_ycs = " << true_ycs << "; amount_per = " << amount_per;
  }

  DistributeRecruits(amount_per);
}

/**
 * Default recruit distribution: put recruits into the single age bin, as used by every
 * stock-recruit child except RecruitmentBevertonHolt when running in Length mode (which
 * overrides this to add the Length-bin distribution branch).
 */
void RecruitmentStockRecruit::DistributeRecruits(Double amount_per) {
  for (auto category : partition_) {
    LOG_FINEST() << category->name_ << "; age: " << age_ << "; category->min_age_: " << category->min_age_ << " recruits = " << amount_per << ", proportion of recruits "
                 << proportions_by_category_[category->name_];
    category->data_[age_ - category->min_age_] += amount_per * proportions_by_category_[category->name_];
  }
}

/**
 *  Called in the initialisation phase, this method scales the partition affected by this recruitment event if recruitment is B0 initialised
 */
void RecruitmentStockRecruit::ScalePartition() {
  if (!parameters_.Get(PARAM_B0)->has_been_defined())
    LOG_CODE_ERROR() << "Cannot apply this method as B0 has not been defined";

  have_scaled_partition  = true;
  Double alternative_ssb = derived_quantity_->GetValue(model()->start_year() - ssb_offset_);

  // Look at initialisation phase
  Double SSB = derived_quantity_->GetLastValueFromInitialisation(phase_b0_);
  if (SSB <= 0.0)
    LOG_FATAL() << "SSB from initialisation was '" << SSB << "' which is invalid. Try doing a run with '--loglevel trace' to determine the error";
  LOG_FINEST() << "Last SSB value = " << SSB << " init ssb = " << alternative_ssb;
  Double scalar = b0_ / SSB;
  LOG_FINEST() << "Scalar = " << scalar << " B0 = " << b0_;
  LOG_FINEST() << "R0 = " << scalar;
  r0_ = scalar;

  for (auto& category : partition_) {
    for (unsigned j = 0; j < category->data_.size(); ++j) {
      if (process_profile_ == ProcessProfile::kAge) {
        LOG_FINEST() << "Category " << category->name_ << " Age = " << j + category->min_age_ << " Numbers at age before = " << category->data_[j];
      }
      category->data_[j] *= scalar;
      if (process_profile_ == ProcessProfile::kAge) {
        LOG_FINEST() << "Category " << category->name_ << " Age = " << j + category->min_age_ << " Numbers at age = " << category->data_[j];
      }
    }
  }
  LOG_FINEST() << "R0 = " << r0_;
}

/**
 * Shared "\nspawn_event_year: " line, identical across all three stock-recruit children.
 */
void RecruitmentStockRecruit::AppendSpawnEventYearField(ostringstream& cache) const {
  cache << "\nspawn_event_year: ";
  for (auto iter : spawn_event_years_) cache << iter << " ";
}

/**
 * Shared "true_ycs" / "recruits" / "recruit_event_SSB" block of the (non-tabular) report
 * cache, identical across all three stock-recruit children. Each child writes its own
 * remaining fields (model_year/multiplier columns, ssb_offset, recruit_event_SSB_percent,
 * Length distribution, etc.) around this.
 */
void RecruitmentStockRecruit::AppendCoreReportFields(ostringstream& cache) const {
  cache << "\ntrue_ycs: ";
  for (auto iter : true_ycs_values_) cache << AS_DOUBLE(iter) << " ";
  cache << "\nrecruits: ";
  for (auto iter : recruitment_values_) cache << AS_DOUBLE(iter) << " ";
  cache << "\nrecruit_event_SSB: ";
  for (auto iter : ssb_values_) cache << AS_DOUBLE(iter) << " ";
}

/**
 * Shared tabular report cache body used unchanged by RecruitmentBevertonHolt and
 * RecruitmentRicker (identical column layout, taking the multiplier maps as parameters since
 * those maps live in the subclasses, not the base). RecruitmentBevertonHoltWithDeviations does
 * not call this - its column set differs materially (no SSB_offset column, different casing).
 */
void RecruitmentStockRecruit::FillMultiplierTabularReportCache(ostringstream& cache, bool first_run,
                                                                 const map<unsigned, Double>& standardised_recruitment_multipliers_by_year,
                                                                 const map<unsigned, Double>& recruitment_multipliers_by_year) const {
  if (first_run) {
    vector<unsigned> years = model()->years();

    for (auto iter : standardised_recruitment_multipliers_by_year) cache << "standardised_recruitment_multipliers[" << iter.first << "] ";
    for (auto iter : recruitment_multipliers_by_year) cache << "recruitment_multipliers[" << iter.first << "] ";

    for (auto year : years) {
      unsigned ssb_year = year - ssb_offset_;
      cache << "true_ycs[" << ssb_year << "] ";
    }
    for (auto year : years) {
      cache << "recruits[" << year << "] ";
    }
    for (auto year : years) {
      unsigned ssb_year = year - ssb_offset_;
      cache << "recruit_event_SSB[" << ssb_year << "] ";
    }

    cache << "R0 B0 steepness ";
    if (process_profile_ == ProcessProfile::kAge)
      cache << "SSB_offset ";
    cache << REPORT_EOL;
  }

  for (auto iter : standardised_recruitment_multipliers_by_year) cache << AS_DOUBLE(iter.second) << " ";
  for (auto iter : recruitment_multipliers_by_year) cache << AS_DOUBLE(iter.second) << " ";

  for (auto value : true_ycs_values_) cache << AS_DOUBLE(value) << " ";
  for (auto value : recruitment_values_) cache << AS_DOUBLE(value) << " ";
  for (auto value : ssb_values_) cache << AS_DOUBLE(value) << " ";

  cache << AS_DOUBLE(r0_) << " " << AS_DOUBLE(b0_) << " " << AS_DOUBLE(steepness_) << " ";
  if (process_profile_ == ProcessProfile::kAge)
    cache << ssb_offset_ << " ";
  cache << REPORT_EOL;
}

} // namespace niwa::processes::common
