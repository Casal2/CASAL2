/**
 * @file MortalityInstantaneousRetained.cpp
 * @author Samik Datta and Craig Marsh (samik.datta@niwa.co.nz)
 * @github https://github.com/Zaita
 * @date 26/06/2019
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "MortalityInstantaneousRetained.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include "AgeLengths/AgeLength.h"
#include "Model/Managers.h"
#include "Selectivities/Manager.h"
#include "TimeSteps/Manager.h"
#include "TimeSteps/TimeStep.h"
#include "Utilities/Math.h"

// namespaces
namespace niwa {
namespace processes {
namespace age {
namespace math = niwa::utilities::math;

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g., profiling, yields, projections, etc.)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate LaTeX for the documentation.
 */
MortalityInstantaneousRetained::MortalityInstantaneousRetained(shared_ptr<Model> model) : MortalityFisheryBase(model) {
  process_type_        = ProcessType::kMortality;
  partition_structure_ = PartitionType::kAge;

  catches_table_ = parameters_.BindTable(PARAM_CATCHES, "The table of removals (catch) data");
  method_table_  = parameters_.BindTable(PARAM_METHOD, "The table of method of removal data");
  catches_table_->set_required_columns({PARAM_YEAR}, true);
  method_table_->set_required_columns(
      {PARAM_METHOD, PARAM_CATEGORY, PARAM_SELECTIVITY, PARAM_TIME_STEP, PARAM_U_MAX, PARAM_PENALTY, PARAM_RETAINED_SELECTIVITY, PARAM_DISCARD_MORTALITY_SELECTIVITY}, true);
  method_table_->set_optional_columns({PARAM_AGE_WEIGHT_LABEL});

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The categories for instantaneous mortality")->flag_is_category();
  parameters_.Bind<Double>(PARAM_M, &m_input_, "The natural mortality rates for each category");
  parameters_.Bind<double>(PARAM_TIME_STEP_PROPORTIONS, &time_step_ratios_temp_, "The time step proportions for natural mortality")->set_is_optional(true);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The M-by-age ogives to apply on the categories for natural mortality")
      ->set_alias_labels({PARAM_RELATIVE_M_BY_AGE});

  RegisterAsAddressable(PARAM_M, &m_);
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when bind<>() overloads cannot be used
 *
 * Note: all parameters are populated from configuration files
 */
void MortalityInstantaneousRetained::DoValidate() {
  parameters_.ValidateVector(PARAM_M)
      ->GreaterThanOrEqualTo(0.0)
      ->LessThanOrEqualTo(1.0)
      ->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)
      ->SameNumberOfElementsAs(PARAM_CATEGORIES);
  parameters_.ValidateVector(PARAM_TIME_STEP_PROPORTIONS)->GreaterThanOrEqualTo(0.0)->SumToOne();
  parameters_.ValidateVector(PARAM_SELECTIVITIES)->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)->SameNumberOfElementsAs(PARAM_CATEGORIES);

  BuildFisheryTables({{PARAM_U_MAX, PARAM_RETAINED_SELECTIVITY, PARAM_DISCARD_MORTALITY_SELECTIVITY}, true});
}

/**
 * Parse the columns of a method table row specific to this process
 *
 * @param row The method table row
 * @param fishery The fishery to populate
 */
void MortalityInstantaneousRetained::ParseFisheryColumns(const MethodRow& row, FisheryData& fishery) {
  ParseUMax(row, fishery);
}

/**
 * Check the columns specific to this process are the same on each row of a fishery
 *
 * @param row The method table row
 * @param stored The fishery from the row it was first seen on
 * @param repeat The fishery from this row
 * @param first_row The row the fishery was first seen on
 */
void MortalityInstantaneousRetained::CheckRepeatedFishery(const MethodRow& row, const FisheryData& stored, const FisheryData& repeat, unsigned first_row) {
  CheckRepeatedUMax(row, stored, repeat, first_row);
}

/**
 * Set up a fishery the first time it is seen in the method table
 *
 * @param fishery The fishery, with its catches populated
 */
void MortalityInstantaneousRetained::InitialiseFishery(FisheryData& fishery) {
  fishery.actual_catches_          = fishery.catches_;  // if different
  fishery.retained_catches_        = fishery.catches_;  // set to obs data
  fishery.actual_retained_catches_ = fishery.catches_;  // if different
  fishery.discards_                = fishery.catches_;  // difference of catch and retained
}

/**
 * Parse the per-category selectivities specific to this process
 *
 * @param row The method table row
 * @param category_count The number of categories on this row
 * @param first_fishery_category The index of this row's first entry in fishery_categories_
 */
void MortalityInstantaneousRetained::ParseFisheryCategoryColumns(const MethodRow& row, unsigned category_count, unsigned first_fishery_category) {
  vector<string> retained_selectivities;
  vector<string> discard_mortality_selectivities;

  boost::split(retained_selectivities, row[PARAM_RETAINED_SELECTIVITY], boost::is_any_of(","));
  boost::split(discard_mortality_selectivities, row[PARAM_DISCARD_MORTALITY_SELECTIVITY], boost::is_any_of(","));

  if (category_count != retained_selectivities.size())
    LOG_FATAL_P(PARAM_METHOD) << "The number of categories (" << category_count << ") and retained selectivities (" << retained_selectivities.size()
                              << ") provided must be the same";
  if (category_count != discard_mortality_selectivities.size())
    LOG_FATAL_P(PARAM_METHOD) << "The number of categories (" << category_count << ") and discard mortality selectivities (" << discard_mortality_selectivities.size()
                              << ") provided must be the same";

  for (unsigned i = 0; first_fishery_category + i < fishery_categories_.size(); ++i) {
    fishery_categories_[first_fishery_category + i].retained_selectivity_label_          = retained_selectivities[i];
    fishery_categories_[first_fishery_category + i].discard_mortality_selectivity_label_ = discard_mortality_selectivities[i];
  }
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 *
 * Validate any parameters that require information from other objects
 */
void MortalityInstantaneousRetained::DoBuild() {
  LOG_TRACE();
  BindCategories();

  for (auto& category : categories_) {
    category.exploitation_.assign(category.category_->age_spread(), 0.0);
    category.exp_values_half_m_.assign(category.category_->age_spread(), 0.0);
  }

  for (auto& fishery_category : fishery_categories_) {
    fishery_category.retained_selectivity_values_.assign(fishery_category.category_.category_->age_spread(), 0.0);
    fishery_category.discard_mortality_selectivity_values_.assign(fishery_category.category_.category_->age_spread(), 0.0);
  }

  BuildTimeStepRatios(true);
  ResolveFisheries();

  for (auto& fishery_category : fishery_categories_) {
    fishery_category.retained_selectivity_ = model()->managers()->selectivity()->GetSelectivity(fishery_category.retained_selectivity_label_);
    if (!fishery_category.retained_selectivity_)
      LOG_ERROR_P(PARAM_METHOD) << ": Retained selectivity " << fishery_category.retained_selectivity_label_ << " was not found.";

    fishery_category.discard_mortality_selectivity_ = model()->managers()->selectivity()->GetSelectivity(fishery_category.discard_mortality_selectivity_label_);
    if (!fishery_category.discard_mortality_selectivity_)
      LOG_ERROR_P(PARAM_METHOD) << ": Discard mortality selectivity " << fishery_category.discard_mortality_selectivity_label_ << " was not found.";
  }

  ResolveCategories();
  BuildSkippedFTimeSteps(PARAM_MORTALITY_INSTANTANEOUS_RETAINED);

  // reserve memory for reporting objects
  removals_by_category_age_.resize(category_labels_.size());
  retained_by_category_age_.resize(category_labels_.size());
  discards_by_category_age_.resize(category_labels_.size());

  for (unsigned i = 0; i < category_labels_.size(); ++i) {
    removals_by_category_age_[i].resize(model()->age_spread());
    retained_by_category_age_[i].resize(model()->age_spread());
    discards_by_category_age_[i].resize(model()->age_spread());
  }

  LOG_FINE() << "years " << process_years_.size();
  // allocate memory for observation object
  const vector<TimeStep*> ordered_time_steps = model()->managers()->time_step()->ordered_time_steps();
  LOG_FINE() << "time steps = " << ordered_time_steps.size();
  LOG_FINE() << "partitions = " << partition_.size();
  for (auto year : process_years_) {
    for (unsigned current_time_step = 0; current_time_step < ordered_time_steps.size(); ++current_time_step) {
      for (auto& category : category_labels_) {
        for (auto& fishery_category : fishery_categories_) {
          LOG_FINE() << "Checking category " << category << " time step = " << fisheries_[fishery_category.fishery_label_].time_step_index_;
          if (fishery_category.category_label_ == category && fisheries_[fishery_category.fishery_label_].time_step_index_ == current_time_step) {
            removals_by_year_fishery_category_[year][fishery_category.fishery_label_][category].assign(model()->age_spread(), 0.0);
            retained_by_year_fishery_category_[year][fishery_category.fishery_label_][category].assign(model()->age_spread(), 0.0);
            discards_by_year_fishery_category_[year][fishery_category.fishery_label_][category].assign(model()->age_spread(), 0.0);
            discard_mortality_by_year_fishery_category_[year][fishery_category.fishery_label_][category].assign(model()->age_spread(), 0.0);

            LOG_FINE() << "year " << year << " fishery = " << fishery_category.fishery_label_ << " category = " << fishery_category.category_label_
                       << "  size of vector = " << removals_by_year_fishery_category_[year][fishery_category.fishery_label_][category].size();
          }
        }
      }
    }
  }
}

/**
 * Execute this process
 */
void MortalityInstantaneousRetained::DoExecute() {
  LOG_TRACE();

  unsigned time_step_index   = model()->managers()->time_step()->current_time_step();
  unsigned year              = model()->current_year();
  Double   ratio             = time_step_ratios_[time_step_index];
  Double   selectivity_value = 0.0;

  for (auto& category : categories_) {
    // Is this category used?
    category.used_in_current_timestep_ = false;
    bool used                          = false;
    for (auto& fishery_category : fishery_categories_) {
      if (fishery_category.category_label_ == category.category_label_ && fishery_category.fishery_.time_step_index_ == time_step_index) {
        used                               = true;
        category.used_in_current_timestep_ = used;
      }
      LOG_FINEST() << "category.category_label_ = " << category.category_label_ << ", fishery_category.category_label_ = " << fishery_category.category_label_
                   << ", fishery_.time_step_index_ = " << fishery_category.fishery_.time_step_index_ << ", time_step_index = " << time_step_index
                   << ", category.used_in_current_timestep_ = " << category.used_in_current_timestep_;

      for (unsigned i = 0; i < category.category_->age_spread(); ++i) {
        selectivity_value               = category.selectivity_->GetAgeResult(category.category_->min_age_ + i, category.category_->age_length_);
        category.exploitation_[i]       = 0.0;
        category.selectivity_values_[i] = selectivity_value;
        // if (used)
        category.exp_values_half_m_[i] = exp(-0.5 * ratio * (*category.m_) * selectivity_value);
      }
    }
  }

  for (auto& fishery_category : fishery_categories_) {
    if (fishery_category.fishery_.time_step_index_ != time_step_index)
      continue;

    for (unsigned i = 0; i < fishery_category.selectivity_values_.size(); ++i)
      fishery_category.selectivity_values_[i]
          = fishery_category.selectivity_->GetAgeResult(fishery_category.category_.category_->min_age_ + i, fishery_category.category_.category_->age_length_);
    for (unsigned i = 0; i < fishery_category.retained_selectivity_values_.size(); ++i)
      fishery_category.retained_selectivity_values_[i]
          = fishery_category.retained_selectivity_->GetAgeResult(fishery_category.category_.category_->min_age_ + i, fishery_category.category_.category_->age_length_);
    for (unsigned i = 0; i < fishery_category.discard_mortality_selectivity_values_.size(); ++i)
      fishery_category.discard_mortality_selectivity_values_[i]
          = fishery_category.discard_mortality_selectivity_->GetAgeResult(fishery_category.category_.category_->min_age_ + i, fishery_category.category_.category_->age_length_);
  }

  for (auto& fishery : fisheries_) {
    fishery.second.total_vulnerability_    = 0.0;
    fishery.second.retained_vulnerability_ = 0.0;
  }

  /**
   * Loop for each category. Add the vulnerability from each
   * category in to the fisheries it belongs too
   */
  if ((model()->state() != State::kInitialise
       || (find(time_steps_to_skip_applying_F_mortality_.begin(), time_steps_to_skip_applying_F_mortality_.end(), time_step_index)
           != time_steps_to_skip_applying_F_mortality_.end()))
      && ((find(process_years_.begin(), process_years_.end(), year) != process_years_.end()) || (year > model()->final_year()))) {
    LOG_FINEST() << "time step = " << time_step_index << " not in initialisation and there is an F method in this timestep. year = " << model()->current_year();
    for (auto& fishery_category : fishery_categories_) {
      LOG_FINEST() << "checking fishery = " << fishery_category.fishery_label_;

      if (fishery_category.fishery_.time_step_index_ != time_step_index)
        continue;

      partition::Category* category = fishery_category.category_.category_;

      Double vulnerable = 0.0;
      if (fishery_category.category_.age_weight_) {
        for (unsigned i = 0; i < category->data_.size(); ++i) {
          vulnerable = category->data_[i] * fishery_category.category_.age_weight_->mean_weight_at_age_by_year(year, i + model()->min_age())
                       * fishery_category.selectivity_values_[i] * fishery_category.category_.exp_values_half_m_[i];

          fishery_category.fishery_.total_vulnerability_ += vulnerable;
          fishery_category.fishery_.retained_vulnerability_ += vulnerable * fishery_category.retained_selectivity_values_[i];
        }
      } else {
        for (unsigned i = 0; i < category->data_.size(); ++i) {
          LOG_FINEST() << "i = " << i << " selectivity = " << fishery_category.selectivity_values_[i] << " numbers at age = " << category->data_[i]
                       << " exp values half m = " << fishery_category.category_.exp_values_half_m_[i]
                       << " mean weight = " << category->age_length_->mean_weight(time_step_index, category->min_age_ + i);

          vulnerable = category->data_[i] * category->age_length_->mean_weight(time_step_index, category->min_age_ + i) * fishery_category.selectivity_values_[i]
                       * fishery_category.category_.exp_values_half_m_[i];

          fishery_category.fishery_.total_vulnerability_ += vulnerable;
          fishery_category.fishery_.retained_vulnerability_ += vulnerable * fishery_category.retained_selectivity_values_[i];
        }
      }
      LOG_FINEST() << "Category is fished in year " << year << " time_step " << time_step_index << " numbers at age length = " << category->data_.size();
      LOG_FINEST() << "Vulnerable biomass from category " << category->name_ << " contributing to fishery " << fishery_category.fishery_label_ << " = "
                   << fishery_category.fishery_.retained_vulnerability_ << " total vulnerable = " << fishery_category.fishery_.total_vulnerability_;

      fishery_category.fishery_.total_vulnerable_by_year_[year]    = fishery_category.fishery_.total_vulnerability_;
      fishery_category.fishery_.retained_vulnerable_by_year_[year] = fishery_category.fishery_.retained_vulnerability_;
    }

    /**
     * Work out the exploitation rate to remove (catch/vulnerable) for each fishery
     */
    Double exploitation;
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      exploitation  = 0.0;

      // If fishery occurs in this time step calculate exploitation rate
      if (fishery.time_step_index_ == time_step_index) {
        fishery.catches_[year] = fishery.retained_catches_[year] * fishery.total_vulnerability_ / utilities::math::ZeroFun(fishery.retained_vulnerability_);
        exploitation           = fishery.retained_catches_[year] / utilities::math::ZeroFun(fishery.retained_vulnerability_);
        //        exploitation = fishery.catches_[year] / utilities::doublecompare::ZeroFun(fishery.retained_vulnerability_);  // like first process (except with "total_")

        fishery.exploitation_ = exploitation;

        LOG_FINEST() << "Vulnerable retained biomass for fishery " << fishery.label_ << " = " << fishery.retained_vulnerability_
                     << " with retained catch = " << fishery.retained_catches_[model()->current_year()] << " and exploitation = " << exploitation;
        LOG_FINEST() << "Vulnerable total biomass for fishery " << fishery.label_ << " = " << fishery.total_vulnerability_
                     << " with total catch = " << fishery.catches_[model()->current_year()] << " and exploitation = " << exploitation;
      } else if (fishery.time_step_index_ > time_step_index) {
        // reset exploitation for fisheries in subsequent time steps only
        fishery.exploitation_ = exploitation;
      }

      LOG_FINE() << "year = " << year << " time_step = " << time_step_index << " fishery = " << fishery.label_ << " exploitation = " << fishery.exploitation_;
    }

    for (auto& fishery_category : fishery_categories_) {
      if (fishery_category.fishery_.time_step_index_ != time_step_index)
        continue;

      partition::Category* category = fishery_category.category_.category_;
      for (unsigned i = 0; i < category->data_.size(); ++i) {
        //        fishery_category.category_.exploitation_[i] += fishery_category.fishery_.exploitation_ * fishery_category.selectivity_values_[i]; // like first process
        fishery_category.category_.exploitation_[i] += fishery_category.fishery_.exploitation_ * fishery_category.selectivity_values_[i]
                                                       * (fishery_category.retained_selectivity_->GetAgeResult(category->min_age_ + i, category->age_length_)
                                                          + (fishery_category.discard_mortality_selectivity_->GetAgeResult(category->min_age_ + i, category->age_length_))
                                                                * (1 - fishery_category.retained_selectivity_->GetAgeResult(category->min_age_ + i, category->age_length_)));
      }
    }

    /*
     * Calculate u_obs for each fishery, this is defined as the maximum proportion of fish taken from any element of the partition
     * affected by fishery f in this time-step
     */
    bool recalculate_age_exploitation = false;
    LOG_FINEST() << "Size of fishery_categories_ " << fishery_categories_.size();

    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;

      // Don't enter if this fishery is not executed here.
      if (fishery.time_step_index_ != time_step_index)
        continue;

      auto& uobs = fishery.uobs_fishery_;
      uobs       = 0.0;
      for (auto& fishery_category : fishery_categories_) {
        if (fishery_category.fishery_.label_ != fishery.label_)
          continue;

        for (Double age_exploitation : fishery_category.category_.exploitation_) {
          uobs = uobs > age_exploitation ? uobs : age_exploitation;
        }
      }
    }

    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;

      // Don't enter if this fishery is not executed here.
      if (fishery.time_step_index_ != time_step_index)
        continue;

      if (fishery.uobs_fishery_ > fishery.u_max_) {
        /**
         * Rescaling exploitation and applying penalties
         */
        LOG_FINE() << fishery.label_ << " exploitation rate before rescaling = " << fishery.exploitation_ << " uobs = " << fishery.uobs_fishery_;
        fishery.exploitation_
            *= (fishery.u_max_ / fishery.uobs_fishery_);  // This may seem weird to be greater than u_max but later we multiply it by the selectivity which scales it to U_max
        LOG_FINE() << "fishery = " << fishery.label_ << " U_obs = " << fishery.uobs_fishery_ << " and u_max " << fishery.u_max_;
        LOG_FINE() << fishery.label_ << " Rescaled exploitation rate = " << fishery.exploitation_;

        recalculate_age_exploitation           = true;
        fishery.actual_catches_[year]          = fishery.total_vulnerability_ * fishery.exploitation_;
        fishery.exploitation_by_year_[year]    = fishery.exploitation_;
        fishery.actual_retained_catches_[year] = fishery.retained_vulnerability_ * fishery.exploitation_;
        //        fishery.discards_[year] = fishery.total_vulnerability_ * fishery.exploitation_ - fishery.actual_catches_[year];

        if (fishery.penalty_)
          fishery.penalty_->Trigger(fishery.catches_[year], fishery.actual_catches_[year]);
      } else {
        fishery.actual_catches_[year]          = fishery.catches_[year];
        fishery.exploitation_by_year_[year]    = fishery.exploitation_;
        fishery.actual_retained_catches_[year] = fishery.retained_catches_[year];
      }

      fishery.discards_[year]      = fishery.actual_catches_[year] - fishery.actual_retained_catches_[year];  // simpler calculation
      fishery.discards_dead_[year] = 0;                                                                       // reset for loop ahead
    }

    /**
     * recalculate age exploitation if we triggered penalty
     */
    if (recalculate_age_exploitation) {
      for (auto& category : categories_) {
        if (!category.used_in_current_timestep_)
          continue;

        LOG_MEDIUM() << "recalculating age exploitation for category " << category.category_label_ << " in time step " << time_step_index;

        for (unsigned i = 0; i < category.category_->age_spread(); ++i) category.exploitation_[i] = 0.0;
      }

      for (auto& fishery_category : fishery_categories_) {
        partition::Category* category = fishery_category.category_.category_;

        // Don't enter if this fishery is not executed here.
        if (fishery_category.fishery_.time_step_index_ != time_step_index)
          continue;

        LOG_MEDIUM() << "updating category exploitation with fishery " << fishery_category.fishery_.label_ << " in time step " << time_step_index
                     << ": exploitation = " << fishery_category.fishery_.exploitation_;

        for (unsigned i = 0; i < category->data_.size(); ++i) {
          //          fishery_category.category_.exploitation_[i] += fishery_category.fishery_.exploitation_ * fishery_category.selectivity_values_[i]; // like first process
          fishery_category.category_.exploitation_[i] += fishery_category.fishery_.exploitation_ * fishery_category.selectivity_values_[i]
                                                         * (fishery_category.retained_selectivity_->GetAgeResult(category->min_age_ + i, category->age_length_)
                                                            + (fishery_category.discard_mortality_selectivity_->GetAgeResult(category->min_age_ + i, category->age_length_))
                                                                  * (1 - fishery_category.retained_selectivity_->GetAgeResult(category->min_age_ + i, category->age_length_)));
        }
      }
    }

    /**
     * Calculate the expectation for a proportions_at_age observation
     */
    LOG_FINEST() << "Calculate proportions at age";
    if (find(process_years_.begin(), process_years_.end(), year) != process_years_.end()) {
      // only calculate observed fits for years the process exists for
      unsigned age_spread      = model()->age_spread();
      for (auto& categories : partition_) {
        for (auto& fishery_category : fishery_categories_) {
          if (fishery_category.category_label_ == categories->name_ && fisheries_[fishery_category.fishery_label_].time_step_index_ == time_step_index) {
            removals_by_year_fishery_category_[year][fishery_category.fishery_label_][categories->name_].assign(age_spread, 0.0);
            retained_by_year_fishery_category_[year][fishery_category.fishery_label_][categories->name_].assign(age_spread, 0.0);
            discards_by_year_fishery_category_[year][fishery_category.fishery_label_][categories->name_].assign(age_spread, 0.0);
            discard_mortality_by_year_fishery_category_[year][fishery_category.fishery_label_][categories->name_].assign(age_spread, 0.0);

            for (unsigned i = 0; i < age_spread; ++i) {
              unsigned age_offset = categories->min_age_ - model()->min_age();
              if (i < age_offset)
                continue;

              removals_by_year_fishery_category_[year][fishery_category.fishery_label_][categories->name_][i]
                  = categories->data_[i - age_offset] * fishery_category.fishery_.exploitation_
                    * fishery_category.selectivity_->GetAgeResult(categories->min_age_ + i, categories->age_length_) * fishery_category.category_.exp_values_half_m_[i];
              // * exp(-0.5 * ratio * m_[categories->name_] * selectivities_[category_offset]->GetAgeResult(categories->min_age_ + i, categories->age_length_));

              retained_by_year_fishery_category_[year][fishery_category.fishery_label_][categories->name_][i]
                  = removals_by_year_fishery_category_[year][fishery_category.fishery_label_][categories->name_][i]
                    * fishery_category.retained_selectivity_->GetAgeResult(categories->min_age_ + i, categories->age_length_);

              discards_by_year_fishery_category_[year][fishery_category.fishery_label_][categories->name_][i]
                  = removals_by_year_fishery_category_[year][fishery_category.fishery_label_][categories->name_][i]
                    - retained_by_year_fishery_category_[year][fishery_category.fishery_label_][categories->name_][i];

              discard_mortality_by_year_fishery_category_[year][fishery_category.fishery_label_][categories->name_][i]
                  = fishery_category.discard_mortality_selectivity_values_[i] * discards_by_year_fishery_category_[year][fishery_category.fishery_label_][categories->name_][i];
            }
          }
        }
      }

      // Record discard mortality per year
      Double fishing_summation    = 0;                                     // dummy variable for summing up contribution from different fisheries to age group
      Double total_catch_by_age   = 0;                                     // dummy variable for viewing total catch
      Double discards_dead_by_age = 0;                                     // dummy variable for viewing total catch
      for (auto& category : categories_) {                                 // over all categories (e.g. male / female, immature / mature)
        for (unsigned i = 0; i < category.category_->data_.size(); ++i) {  // over all ages
          LOG_FINEST() << "Age " << i + model()->min_age();
          for (auto& fishery_category : fishery_categories_) {  // over all fisheries
            if (fishery_category.category_label_ == category.category_label_
                && fishery_category.fishery_.time_step_index_ == time_step_index) {  // if fishing occurs on this category at this time
              LOG_FINEST() << "Numbers before: " << category.category_->data_[i];
              LOG_FINEST() << "category.selectivity_values_[i] = " << category.selectivity_values_[i];
              LOG_FINEST() << "fishery_category.selectivity_values_[i] = " << fishery_category.selectivity_values_[i];
              LOG_FINEST() << "fishery_category.fishery_.exploitation_ = " << fishery_category.fishery_.exploitation_;
              LOG_FINEST() << "mean weight by age = " << fishery_category.category_.category_->age_length_->mean_weight(time_step_index, i + model()->min_age());

              total_catch_by_age = category.category_->data_[i] * category.exp_values_half_m_[i] * fishery_category.selectivity_values_[i] * fishery_category.fishery_.exploitation_
                                   * fishery_category.category_.category_->age_length_->mean_weight(time_step_index, i + model()->min_age());
              LOG_FINE() << "Total catch: " << total_catch_by_age;

              discards_dead_by_age = fishery_category.discard_mortality_selectivity_values_[i] * total_catch_by_age
                                     * (1 - fishery_category.retained_selectivity_values_[i]);  // correct calculation?
              LOG_FINE() << "Discards dead: " << discards_dead_by_age;

              fishery_category.fishery_.discards_dead_[year] += discards_dead_by_age;
              fishing_summation += fishery_category.selectivity_values_[i] * fishery_category.fishery_.exploitation_
                                   * (fishery_category.retained_selectivity_values_[i]
                                      + fishery_category.discard_mortality_selectivity_values_[i] * (1 - fishery_category.retained_selectivity_values_[i]));
              LOG_FINEST() << "Total fishing summation: " << fishing_summation;
            }
          }
        }
      }
    }
  }  // if (model_->state() != State::kInitialise )

  /**
   * Remove the stock now using the exploitation rate
   */
  for (auto& category : categories_) {
    LOG_MEDIUM() << "category " << category.category_label_ << " used in time step " << time_step_index << ": " << category.used_in_current_timestep_;

    for (unsigned i = 0; i < category.category_->data_.size(); ++i) {
      // removals_by_category_age_[category_ndx][i] = category.category_->data_[i]; // initial numbers before process
      LOG_FINEST() << "category " << category.category_label_ << ": numbers at age = " << category.category_->data_[i] << " age " << i + model()->min_age()
                   << " exploitation = " << category.exploitation_[i] << " M = " << *category.m_ << " ratio = " << ratio
                   << " relative_m_by_age = " << category.selectivity_values_[i];

      category.category_->data_[i] *= category.exp_values_half_m_[i] * category.exp_values_half_m_[i] * (1 - category.exploitation_[i]);
      LOG_FINEST() << "category " << category.category_label_ << ": updated numbers at age = " << category.category_->data_[i] << " age " << i + model()->min_age();

      if (category.category_->data_[i] < 0.0) {
        LOG_CODE_ERROR() << " Fishing caused a negative partition : if (categories->data_[i] < 0.0), category.category_->data_[i] = " << category.category_->data_[i]
                         << " i = " << i + 1 << "; category " << category.category_label_ << ": numbers at age = " << category.category_->data_[i] << " age "
                         << i + model()->min_age() << " exploitation = " << category.exploitation_[i] << " relative_m_by_age = " << category.selectivity_values_[i]
                         << " M = " << *category.m_ << " ratio = " << ratio << " time step = " << time_step_index << " used in time step = " << category.used_in_current_timestep_;
      }
      // removals_by_category_age_[category_ndx][i] -= category.category_->data_[i]; // minus what was left thus keeping the difference
    }
  }
  // removals_by_year_category_age_[model_->current_year()] = removals_by_category_age_;
}

/*
 * Fill the report cache
 * @description A method for reporting process information
 * @param cache a cache object to print to
 */
void MortalityInstantaneousRetained::FillReportCache(ostringstream& cache) {
  LOG_FINE();
  // This one is niggly because we need to iterate over each year and time step to print the right information so we don't
  vector<unsigned> years = model()->years();

  cache << "year: ";
  for (auto year : years) cache << year << " ";

  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    cache << "\nfishing_pressure[" << fishery.label_ << "]: ";
    for (auto pressure : fishery.exploitation_by_year_) cache << AS_DOUBLE(pressure.second) << " ";
    cache << "\ncatch[" << fishery.label_ << "]: ";
    for (auto catches : fishery.catches_) cache << AS_DOUBLE(catches.second) << " ";
    cache << "\nactual_catch[" << fishery.label_ << "]: ";
    for (auto catches : fishery.actual_catches_) cache << AS_DOUBLE(catches.second) << " ";
    cache << "\nretained_catch[" << fishery.label_ << "]: ";
    for (auto retained : fishery.retained_catches_) cache << AS_DOUBLE(retained.second) << " ";
    cache << "\nactual_retained_catch[" << fishery.label_ << "]: ";
    for (auto retained : fishery.actual_retained_catches_) cache << AS_DOUBLE(retained.second) << " ";
    cache << "\ndiscards[" << fishery.label_ << "]: ";
    for (auto discards : fishery.discards_) cache << AS_DOUBLE(discards.second) << " ";
    cache << "\ndiscards_dead[" << fishery.label_ << "]: ";
    for (auto disc_dead : fishery.discards_dead_) cache << AS_DOUBLE(disc_dead.second) << " ";
  }

  cache << REPORT_EOL;

  //  // Print discard info
  //  for (auto& fishery_category : fishery_categories_) {
  //    cache << "total_discard_age_freq_" << fishery_category.fishery_label_ << "-" << fishery_category.category_label_ << " " << REPORT_R_DATAFRAME << "\nyear ";
  //    for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age)
  //      cache << age << " ";
  //    cache << REPORT_EOL;
  //    for (auto& year_values : total_discards_by_year_fishery_category_) {
  //      for (auto& fishery_values : year_values.second) {
  //        for (auto& category_values : fishery_values.second) {
  //          if ((category_values.first == fishery_category.category_label_) && (fishery_values.first == fishery_category.fishery_label_)) {
  //            cache << year_values.first << " ";
  //            for (auto age_vals : category_values.second) {
  //              cache << age_vals << " ";
  //            }
  //          }
  //        }
  //      }
  //      cache << REPORT_EOL;
  //    }
  //  }
  //
  //  // Print surviving discard info
  //  for (auto& fishery_category : fishery_categories_) {
  //    cache << "surviving_discard_age_freq_" << fishery_category.fishery_label_ << "-" << fishery_category.category_label_ << " " << REPORT_R_DATAFRAME << "\nyear ";
  //    for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age)
  //      cache << age << " ";
  //    cache << REPORT_EOL;
  //    for (auto& year_values : surviving_discards_by_year_fishery_category_) {
  //      for (auto& fishery_values : year_values.second) {
  //        for (auto& category_values : fishery_values.second) {
  //          if ((category_values.first == fishery_category.category_label_) && (fishery_values.first == fishery_category.fishery_label_)) {
  //            cache << year_values.first << " ";
  //            for (auto age_vals : category_values.second) {
  //              cache << age_vals << " ";
  //            }
  //          }
  //        }
  //      }
  //      cache << REPORT_EOL;
  //    }
  //  }
  //
  //
  //  // Print retained age-freq info
  //  for (auto& fishery_category : fishery_categories_) {
  //    cache << "retained_age_freq_" << fishery_category.fishery_label_ << "-" << fishery_category.category_label_ << " " << REPORT_R_DATAFRAME << "\nyear ";
  //    for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age)
  //      cache << age << " ";
  //    cache << REPORT_EOL;
  //    for (auto& year_values : removals_by_year_fishery_category_) {
  //      for (auto& fishery_values : year_values.second) {
  //        for (auto& category_values : fishery_values.second) {
  //          if ((category_values.first == fishery_category.category_label_) && (fishery_values.first == fishery_category.fishery_label_)) {
  //            cache << year_values.first << " ";
  //            for (auto age_vals : category_values.second) {
  //              cache << age_vals << " ";
  //            }
  //          }
  //        }
  //      }
  //      cache << REPORT_EOL;
  //    }
  //  }
  //  cache << REPORT_EOL;

  /*
    cache << "removals " << REPORT_R_DATAFRAME << "\n";
    cache << "year category";
    unsigned age = model()->min_age();
    while(age <= model()->max_age()) {
      cache << " " << age ;
      ++age;
    }
    cache << REPORT_EOL;

    for (auto& removals : removals_by_year_category_age_) {
      LOG_FINE() << "printing year = " << removals.first;
      for (unsigned category_ndx = 0; category_ndx < removals.second.size(); ++category_ndx) {
        cache << removals.first << " " << category_labels_[category_ndx];
        for (unsigned age_ndx = 0; age_ndx < removals.second[category_ndx].size(); ++age_ndx) {
          cache << " " << removals.second[category_ndx][age_ndx];
        }
        cache << REPORT_EOL;
      }
    }
  */
}

/**
 * Fill the tabular report cache
 * @description A method for reporting tabular process information
 * @param cache a cache object to print to
 * @param first_run whether to print the header
 */
void MortalityInstantaneousRetained::FillTabularReportCache(ostringstream& cache, bool first_run) {
  if (first_run) {
    // print header
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      for (auto pressure : fishery.exploitation_by_year_) cache << "fishing_pressure[" << fishery.label_ << "][" << pressure.first << "] ";
      for (auto catches : fishery.catches_) cache << "catch[" << fishery.label_ << "][" << catches.first << "] ";
      for (auto actual_catches : fishery.actual_catches_) cache << "actual_catches[" << fishery.label_ << "][" << actual_catches.first << "] ";
      // TODO:  check this block
      for (auto retained : fishery.retained_catches_) cache << "retained_catch[" << fishery.label_ << "][" << retained.first << "] ";
      for (auto retained : fishery.actual_retained_catches_) cache << "actual_retained_catch[" << fishery.label_ << "][" << retained.first << "] ";
      for (auto discards : fishery.discards_) cache << "discards[" << fishery.label_ << "][" << discards.first << "] ";
      for (auto disc_dead : fishery.discards_dead_) cache << "discards_dead[" << fishery.label_ << "][" << disc_dead.first << "] ";
    }
    cache << REPORT_EOL;
  }

  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    for (auto pressure : fishery.exploitation_by_year_) cache << AS_DOUBLE(pressure.second) << " ";
    for (auto catches : fishery.catches_) cache << AS_DOUBLE(catches.second) << " ";
    for (auto actual_catches : fishery.actual_catches_) cache << AS_DOUBLE(actual_catches.second) << " ";
    // TODO:  check this block
    for (auto retained : fishery.retained_catches_) cache << AS_DOUBLE(retained.second) << " ";
    for (auto retained : fishery.actual_retained_catches_) cache << AS_DOUBLE(retained.second) << " ";
    for (auto discards : fishery.discards_) cache << AS_DOUBLE(discards.second) << " ";
    for (auto disc_dead : fishery.discards_dead_) cache << AS_DOUBLE(disc_dead.second) << " ";
  }
  cache << REPORT_EOL;
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
