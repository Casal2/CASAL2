/**
 * @file MortalityInstantaneous.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/07/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * Consolidated implementation for both Age and Length partitioned models.
 */

// headers
#include "MortalityInstantaneous.h"

#include "AgeLengths/AgeLength.h"
#include "GrowthIncrements/GrowthIncrement.h"
#include "Model/Managers.h"
#include "TimeSteps/Manager.h"
#include "TimeSteps/TimeStep.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"
#include "Utilities/Vector.h"

// namespaces
namespace niwa::processes::common {
using niwa::utilities::findInVector;

/**
 * Default constructor
 */
MortalityInstantaneous::MortalityInstantaneous(shared_ptr<Model> model) : MortalityFisheryBase(model) {
  process_type_        = ProcessType::kMortality;
  partition_structure_ = PartitionType::kAge | PartitionType::kLength;

  catches_table_ = parameters_.BindTable(PARAM_CATCHES, "The table of removals (catch) data");
  catches_table_->set_required_columns({PARAM_YEAR}, true);
  method_table_ = parameters_.BindTable(PARAM_METHOD, "The table of method of removal data");
  // Note: Not using set_required_columns here because Age models have optional columns (age_weight_label)
  // Columns are validated manually in DoValidate

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The categories for instantaneous mortality")->flag_is_category();
  parameters_.Bind<Double>(PARAM_M, &m_input_, "The natural mortality rates for each category");
  parameters_.Bind<double>(PARAM_TIME_STEP_PROPORTIONS, &time_step_ratios_temp_, "The time step proportions for natural mortality");
  parameters_.Bind<bool>(PARAM_BIOMASS, &is_catch_biomass_, "Indicator to denote if the catches are as biomass (true) or abundance (false)")->set_default_value(true);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The M-by-age/length ogives to apply to each category for natural mortality")
      ->set_alias_labels({PARAM_RELATIVE_M_BY_AGE, PARAM_RELATIVE_M_BY_LENGTH});

  RegisterAsAddressable(PARAM_M, &m_);
}

/**
 * Validate the process
 */
void MortalityInstantaneous::DoValidate() {
  if (process_profile_ == ProcessProfile::kAge) {
    parameters_.ValidateVector(PARAM_M)
        ->GreaterThanOrEqualTo(0.0)
        ->LessThanOrEqualTo(1.0)
        ->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)
        ->SameNumberOfElementsAs(PARAM_CATEGORIES);
  } else {
    parameters_.ValidateVector(PARAM_M)->GreaterThanOrEqualTo(0.0)->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)->SameNumberOfElementsAs(PARAM_CATEGORIES);
  }
  parameters_.ValidateVector(PARAM_TIME_STEP_PROPORTIONS)->GreaterThanOrEqualTo(0.0)->LessThanOrEqualTo(1.0)->SumToOne();
  parameters_.ValidateVector(PARAM_SELECTIVITIES)->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)->SameNumberOfElementsAs(PARAM_CATEGORIES);

  // Age-specific optional columns
  auto method_columns = method_table_->columns();
  use_catch_biomass_  = process_profile_ == ProcessProfile::kAge && std::find(method_columns.begin(), method_columns.end(), PARAM_BIOMASS) != method_columns.end();
  use_u_              = process_profile_ == ProcessProfile::kAge && std::find(method_columns.begin(), method_columns.end(), PARAM_U) != method_columns.end();

  BuildFisheryTables({{PARAM_U_MAX}});
}

/**
 * Parse the columns of a method table row specific to this process
 *
 * @param row The method table row
 * @param fishery The fishery to populate
 */
void MortalityInstantaneous::ParseFisheryColumns(const MethodRow& row, FisheryData& fishery) {
  ParseUMax(row, fishery);

  // Age-specific: parse biomass and U flags
  if (process_profile_ == ProcessProfile::kAge) {
    if (use_catch_biomass_) {
      if (!(utilities::To(row[PARAM_BIOMASS], fishery.catch_as_biomass_)))
        LOG_ERROR_P(PARAM_METHOD) << ": in row = " << row.number_ << ", unable to convert '" << row[PARAM_BIOMASS] << "' to a boolean (true/false)";
      LOG_FINE() << "row[PARAM_BIOMASS] = " << row[PARAM_BIOMASS] << ", fishery.catch_as_biomass_ = " << fishery.catch_as_biomass_;
    } else {
      fishery.catch_as_biomass_ = true;
    }
    if (use_u_) {
      if (!(utilities::To(row[PARAM_U], fishery.catch_as_u_)))
        LOG_ERROR_P(PARAM_METHOD) << ": in row = " << row.number_ << ", unable to convert '" << row[PARAM_U] << "' to a boolean (true/false) ";
    } else {
      fishery.catch_as_u_ = false;
    }
  }
}

/**
 * Check the columns specific to this process are the same on each row of a fishery
 *
 * @param row The method table row
 * @param stored The fishery from the row it was first seen on
 * @param repeat The fishery from this row
 * @param first_row The row the fishery was first seen on
 */
void MortalityInstantaneous::CheckRepeatedFishery(const MethodRow& row, const FisheryData& stored, const FisheryData& repeat, unsigned first_row) {
  CheckRepeatedUMax(row, stored, repeat, first_row);

  if (process_profile_ == ProcessProfile::kAge) {
    if (stored.catch_as_biomass_ != repeat.catch_as_biomass_)
      LOG_ERROR_P(PARAM_METHOD) << "Fishery labelled " << repeat.label_ << " had biomass = " << repeat.catch_as_biomass_ << " at row " << row.number_
                                << ", but the same fishery had biomass = " << stored.catch_as_biomass_ << " at row " << first_row
                                << ". These need to be the same for a single method in the methods table";
    if (stored.catch_as_u_ != repeat.catch_as_u_)
      LOG_ERROR_P(PARAM_METHOD) << "Fishery labelled " << repeat.label_ << " had U = " << repeat.catch_as_u_ << " at row " << row.number_
                                << ", but the same fishery had U = " << stored.catch_as_u_ << " at row " << first_row
                                << ". These need to be the same for a single method in the methods table";
  }
}

/**
 * Set up a fishery the first time it is seen in the method table
 *
 * @param fishery The fishery, with its catches populated
 */
void MortalityInstantaneous::InitialiseFishery(FisheryData& fishery) {
  fishery.actual_catches_ = fishery.catches_;

  // Age-specific: check exploitation rate values if catch_as_u
  if (process_profile_ == ProcessProfile::kAge && fishery.catch_as_u_) {
    for (auto iter : fishery.catches_) {
      if (iter.second < 0.0 || iter.second > 1.0) {
        LOG_ERROR_P(PARAM_CATCHES) << ": the column " << fishery.label_ << " with year " << iter.first << " has value '" << iter.second
                                   << "'. It must have a value between 0 and 1 (inclusive) as the column is defined as an exploitation rate in the " << PARAM_METHOD
                                   << " table";
      }
    }
  }
}

/**
 * Build any objects that will need to be utilised by this object.
 */
void MortalityInstantaneous::DoBuild() {
  LOG_TRACE();
  BindCategories();

  for (auto& category : categories_) {
    unsigned category_bin_count = CategoryBinCount(category);
    category.exploitation_.assign(category_bin_count, 0.0);
    category.exp_values_half_m_.assign(category_bin_count, 0.0);
    if (process_profile_ == ProcessProfile::kAge)
      category.m_values_.assign(category_bin_count, 0.0);
  }

  BuildTimeStepRatios(false);
  ResolveFisheries();
  ResolveCategories();
  BuildSkippedFTimeSteps(PARAM_MORTALITY_INSTANTANEOUS);
  AllocateRemovals((process_profile_ == ProcessProfile::kAge) ? model()->age_spread() : model()->get_number_of_length_bins());
}

/**
 * Execute this process
 */
void MortalityInstantaneous::DoExecute() {
  LOG_TRACE();
  unsigned             time_step_index = model()->managers()->time_step()->current_time_step();
  unsigned             year            = model()->current_year();
  double               ratio           = time_step_ratios_[time_step_index];
  std::pair<bool, int> this_year_iter  = findInVector(process_years_, year);

  if (this_year_iter.first) {
    LOG_FINE() << "Year = " << year << " found? = " << this_year_iter.first << " should = " << process_years_[this_year_iter.second];
  } else {
    LOG_FINE() << "Year = " << year << " found? = " << this_year_iter.first;
  }

  Double selectivity_value = 0.0;

  // Pre-calculate M values and exp(-0.5*M*ratio)
  for (auto& category : categories_) {
    unsigned data_size = category.category_->data_.size();
    for (unsigned i = 0; i < data_size; ++i) {
      if (process_profile_ == ProcessProfile::kAge) {
        selectivity_value              = category.selectivity_->GetAgeResult(category.category_->min_age_ + i, category.category_->age_length_);
        category.m_values_[i]          = (*category.m_) * selectivity_value;
        category.exp_values_half_m_[i] = exp(-0.5 * ratio * category.m_values_[i]);
      } else {
        selectivity_value              = category.selectivity_->GetLengthResult(i);
        category.exp_values_half_m_[i] = exp(-0.5 * ratio * (*category.m_) * selectivity_value);
      }
      category.exploitation_[i]       = 0.0;
      category.selectivity_values_[i] = selectivity_value;
      LOG_FINEST() << "category " << category.category_label_ << " index " << i << " selectivity " << selectivity_value;
    }
  }

  for (auto& fishery : fisheries_) fishery.second.vulnerability_ = 0.0;

  /**
   * This is where F gets applied
   */
  bool apply_fishing = false;
  if (model()->state() != State::kInitialise) {
    if (((find(process_years_.begin(), process_years_.end(), year) != process_years_.end()) || (year > model()->final_year()))) {
      if (find(time_steps_to_skip_applying_F_mortality_.begin(), time_steps_to_skip_applying_F_mortality_.end(), time_step_index)
          == time_steps_to_skip_applying_F_mortality_.end()) {
        apply_fishing = true;
      }
    }
  } else if (process_profile_ == ProcessProfile::kLength) {
    // Length model: different initialisation check
    if ((find(time_steps_to_skip_applying_F_mortality_.begin(), time_steps_to_skip_applying_F_mortality_.end(), time_step_index) != time_steps_to_skip_applying_F_mortality_.end())
        && ((find(process_years_.begin(), process_years_.end(), year) != process_years_.end()) || (year > model()->final_year()))) {
      apply_fishing = true;
    }
  }

  if (apply_fishing) {
    LOG_FINEST() << "time step = " << time_step_index << " not in initialisation and there is an F method in this timestep. year = " << model()->current_year();

    // Calculate selectivity values for fishery categories
    for (auto& fishery_category : fishery_categories_) {
      if (fishery_category.fishery_.time_step_index_ != time_step_index)
        continue;

      for (unsigned i = 0; i < fishery_category.selectivity_values_.size(); ++i) {
        if (process_profile_ == ProcessProfile::kAge) {
          fishery_category.selectivity_values_[i]
              = fishery_category.selectivity_->GetAgeResult(fishery_category.category_.category_->min_age_ + i, fishery_category.category_.category_->age_length_);
        } else {
          fishery_category.selectivity_values_[i] = fishery_category.selectivity_->GetLengthResult(i);
        }
        LOG_FINEST() << "fishery category " << fishery_category.fishery_label_ << " index " << i << " selectivity " << fishery_category.selectivity_values_[i];
      }
    }

    // Calculate vulnerability
    for (auto& fishery_category : fishery_categories_) {
      LOG_FINEST() << "checking fishery = " << fishery_category.fishery_label_;

      if (fishery_category.fishery_.time_step_index_ != time_step_index)
        continue;

      partition::Category* category   = fishery_category.category_.category_;
      Double               vulnerable = 0.0;

      if (process_profile_ == ProcessProfile::kAge) {
        if (fishery_category.category_.age_weight_) {
          for (unsigned i = 0; i < category->data_.size(); ++i) {
            vulnerable += category->data_[i] * fishery_category.category_.age_weight_->mean_weight_at_age_by_year(year, i + model()->min_age())
                          * fishery_category.selectivity_values_[i] * fishery_category.category_.exp_values_half_m_[i];
          }
        } else if (fishery_category.fishery_.catch_as_biomass_) {
          for (unsigned i = 0; i < category->data_.size(); ++i) {
            vulnerable += category->data_[i] * category->age_length_->mean_weight(time_step_index, category->min_age_ + i) * fishery_category.selectivity_values_[i]
                          * fishery_category.category_.exp_values_half_m_[i];
          }
        } else {
          for (unsigned i = 0; i < category->data_.size(); ++i) {
            vulnerable += category->data_[i] * fishery_category.selectivity_values_[i] * fishery_category.category_.exp_values_half_m_[i];
          }
        }
      } else {
        // Length
        if (is_catch_biomass_) {
          for (unsigned i = 0; i < category->data_.size(); ++i) {
            vulnerable += category->data_[i] * category->growth_increment_->get_mean_weight(i) * fishery_category.selectivity_values_[i]
                          * fishery_category.category_.exp_values_half_m_[i];
          }
        } else {
          for (unsigned i = 0; i < category->data_.size(); ++i) {
            vulnerable += category->data_[i] * fishery_category.selectivity_values_[i] * fishery_category.category_.exp_values_half_m_[i];
          }
        }
      }

      fishery_category.fishery_.vulnerability_ += vulnerable;
      LOG_FINEST() << "Vulnerable biomass from category " << category->name_ << " contributing to fishery " << fishery_category.fishery_label_ << " = " << vulnerable;
    }

    /**
     * Work out the exploitation rate to remove (catch/vulnerable) for each fishery
     */
    Double exploitation;
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      exploitation  = 0.0;

      if (fishery.time_step_index_ == time_step_index) {
        if (process_profile_ == ProcessProfile::kAge && fishery.catch_as_u_) {
          exploitation = fishery.catches_[year];
        } else {
          exploitation = fishery.catches_[year] / utilities::math::ZeroFun(fishery.vulnerability_);
        }

        fishery.exploitation_ = exploitation;
        fishery.uobs_fishery_ = exploitation;

        LOG_FINEST() << "Vulnerable biomass for fishery " << fishery.label_ << " = " << fishery.vulnerability_ << " with catch = " << fishery.catches_[year]
                     << " and exploitation = " << exploitation;
      } else if (fishery.time_step_index_ > time_step_index) {
        fishery.exploitation_ = exploitation;
        fishery.uobs_fishery_ = exploitation;
      }
      LOG_FINE() << "year = " << year << " time step = " << time_step_index << " fishery = " << fishery.label_ << " exploitation = " << fishery.exploitation_;
    }

    // Calculate exploitation by category
    for (auto& fishery_category : fishery_categories_) {
      if (fishery_category.fishery_.time_step_index_ != time_step_index)
        continue;

      partition::Category* category = fishery_category.category_.category_;
      for (unsigned i = 0; i < category->data_.size(); ++i) {
        fishery_category.category_.exploitation_[i] += fishery_category.fishery_.exploitation_ * fishery_category.selectivity_values_[i];
      }
    }

    /**
     * Calculate u_obs for each fishery
     */
    bool recalculate_exploitation = false;
    LOG_FINEST() << "calc uobs";

    for (auto& fishery_iter : fisheries_) {
      LOG_FINEST() << "fishery = " << fishery_iter.first;
      auto& fishery = fishery_iter.second;

      if (fishery.time_step_index_ != time_step_index)
        continue;

      auto& uobs = fishery.uobs_fishery_;
      uobs       = 0.0;
      for (auto& fishery_category : fishery_categories_) {
        if (fishery_category.fishery_.label_ != fishery.label_)
          continue;

        for (Double bin_exploitation : fishery_category.category_.exploitation_) {
          uobs = uobs > bin_exploitation ? uobs : bin_exploitation;
        }
      }
    }

    // Check if we need to rescale exploitation
    for (auto& fishery_iter : fisheries_) {
      LOG_FINEST() << "fishery = " << fishery_iter.first;
      auto& fishery = fishery_iter.second;

      if (fishery.time_step_index_ != time_step_index)
        continue;

      if (fishery.uobs_fishery_ > fishery.u_max_) {
        LOG_FINE() << fishery.label_ << " exploitation rate before rescaling = " << fishery.exploitation_ << " uobs = " << fishery.uobs_fishery_;
        fishery.exploitation_ *= (fishery.u_max_ / fishery.uobs_fishery_);
        LOG_FINE() << "fishery = " << fishery.label_ << " U_obs = " << fishery.uobs_fishery_ << " and u_max " << fishery.u_max_;
        LOG_FINE() << fishery.label_ << " rescaled exploitation rate = " << fishery.exploitation_;

        recalculate_exploitation            = true;
        fishery.actual_catches_[year]       = fishery.vulnerability_ * fishery.exploitation_;
        fishery.exploitation_by_year_[year] = fishery.exploitation_;
        if (fishery.penalty_)
          fishery.penalty_->Trigger(fishery.catches_[year], fishery.actual_catches_[year]);
      } else {
        if (process_profile_ == ProcessProfile::kAge && fishery.catch_as_u_) {
          fishery.actual_catches_[year] = fishery.vulnerability_ * fishery.exploitation_;
        } else {
          fishery.actual_catches_[year] = fishery.catches_[year];
        }
        fishery.exploitation_by_year_[year] = fishery.exploitation_;
      }
      fishery.uobs_by_year_[year] = fishery.uobs_fishery_;
    }

    /**
     * Recalculate exploitation if we triggered penalty
     */
    if (recalculate_exploitation) {
      for (auto& category : categories_) {
        LOG_FINE() << "recalculating exploitation for category " << category.category_label_ << " in time step " << time_step_index;
        for (unsigned i = 0; i < category.category_->data_.size(); ++i) category.exploitation_[i] = 0.0;
      }

      for (auto& fishery_category : fishery_categories_) {
        partition::Category* category = fishery_category.category_.category_;

        if (fishery_category.fishery_.time_step_index_ != time_step_index)
          continue;

        LOG_FINE() << "updating category exploitation with fishery " << fishery_category.fishery_.label_ << " in time step " << time_step_index
                   << ": exploitation = " << fishery_category.fishery_.exploitation_;

        for (unsigned i = 0; i < category->data_.size(); ++i) {
          fishery_category.category_.exploitation_[i] += fishery_category.fishery_.exploitation_ * fishery_category.selectivity_values_[i];
        }
      }
    }

    /**
     * Calculate the expectation for a proportions observation
     */
    LOG_FINEST() << "Calculate proportions";
    if (find(process_years_.begin(), process_years_.end(), year) != process_years_.end()) {
      unsigned bin_count = (process_profile_ == ProcessProfile::kAge) ? model()->age_spread() : model()->get_number_of_length_bins();

      if (process_profile_ == ProcessProfile::kAge) {
        for (auto& fishery_category : fishery_categories_) {
          if (fishery_category.fishery_.time_step_index_ != time_step_index)
            continue;
          partition::Category* category = fishery_category.category_.category_;
          LOG_FINEST() << "category = " << category->name_ << " fishery = " << fishery_category.fishery_label_;

          for (unsigned i = 0; i < bin_count; ++i) {
            unsigned age_offset = category->min_age_ - model()->min_age();
            if (i < age_offset)
              continue;
            removals_by_year_fishery_category_[this_year_iter.second][fishery_category.fishery_.fishery_ndx_][fishery_category.category_.category_ndx_][i]
                = category->data_[i - age_offset] * fishery_category.fishery_.exploitation_
                  * fishery_category.selectivity_->GetAgeResult(category->min_age_ + i, category->age_length_) * fishery_category.category_.exp_values_half_m_[i];
          }
        }
      } else {
        for (auto& categories : partition_) {
          for (auto& fishery_category : fishery_categories_) {
            if (fishery_category.category_label_ == categories->name_ && fisheries_[fishery_category.fishery_label_].time_step_index_ == time_step_index) {
              LOG_FINEST() << "category = " << categories->name_ << " fishery = " << fishery_category.fishery_label_;
              for (unsigned i = 0; i < bin_count; ++i) {
                removals_by_year_fishery_category_[this_year_iter.second][fishery_category.fishery_.fishery_ndx_][fishery_category.category_.category_ndx_][i]
                    = categories->data_[i] * fishery_category.fishery_.exploitation_ * fishery_category.selectivity_->GetLengthResult(i)
                      * fishery_category.category_.exp_values_half_m_[i];
              }
            }
          }
        }
      }
    }
  }

  /**
   * Remove the stock now using the exploitation rate
   */
  for (auto& category : categories_) {
    LOG_FINEST() << "category " << category.category_label_ << " used in time step " << time_step_index << ": " << category.used_in_current_timestep_;

    for (unsigned i = 0; i < category.category_->data_.size(); ++i) {
      LOG_FINEST() << "category " << category.category_label_ << ": numbers = " << category.category_->data_[i] << " index " << i << " exploitation = " << category.exploitation_[i]
                   << " M = " << *category.m_ << " selectivity = " << category.selectivity_values_[i];

      category.category_->data_[i] *= category.exp_values_half_m_[i] * category.exp_values_half_m_[i] * (1.0 - category.exploitation_[i]);
      LOG_FINEST() << "category " << category.category_label_ << ": updated numbers = " << category.category_->data_[i] << " index " << i;

      if (category.category_->data_[i] < 0.0) {
        LOG_CODE_ERROR() << " Fishing caused a negative partition : category.category_->data_[i] = " << category.category_->data_[i] << " i = " << i + 1 << "; category "
                         << category.category_label_ << " exploitation = " << category.exploitation_[i] << " selectivity = " << category.selectivity_values_[i]
                         << " M = " << *category.m_ << " time step = " << time_step_index << " used in time step = " << category.used_in_current_timestep_
                         << ". The category.exp_values_half_m_[i] was " << category.exp_values_half_m_[i];
      }
    }
  }
}

/**
 * Fill the report cache
 */
void MortalityInstantaneous::FillReportCache(ostringstream& cache) {
  LOG_FINE();

  // Age-specific: report m_by_age
  if (process_profile_ == ProcessProfile::kAge) {
    for (auto& category : categories_) {
      cache << "m_by_age[" << category.category_label_ << "]: ";
      for (unsigned i = 0; i < category.category_->data_.size(); ++i) {
        cache << AS_DOUBLE(category.m_values_[i]) << " ";
      }
      cache << "\n";
    }
  }

  cache << "year: ";
  for (auto year : process_years_) cache << year << " ";

  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    cache << "\nexploitation_rate[" << fishery.label_ << "]: ";
    for (auto pressure : fishery.exploitation_by_year_) cache << AS_DOUBLE(pressure.second) << " ";
    cache << "\nfishing_pressure[" << fishery.label_ << "]: ";
    for (auto uobs_pressure : fishery.uobs_by_year_) cache << AS_DOUBLE(uobs_pressure.second) << " ";
    cache << "\ncatch[" << fishery.label_ << "]: ";
    for (auto catches : fishery.catches_) cache << AS_DOUBLE(catches.second) << " ";
    cache << "\nactual_catch[" << fishery.label_ << "]: ";
    for (auto actual_catches : fishery.actual_catches_) cache << AS_DOUBLE(actual_catches.second) << " ";
  }

  cache << REPORT_EOL;
}

/**
 * Fill the tabular report cache
 */
void MortalityInstantaneous::FillTabularReportCache(ostringstream& cache, bool first_run) {
  if (first_run) {
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      for (auto pressure : fishery.exploitation_by_year_) cache << "exploitation_rate[" << fishery.label_ << "][" << pressure.first << "] ";
      for (auto pressure : fishery.uobs_by_year_) cache << "fishing_pressure[" << fishery.label_ << "][" << pressure.first << "] ";
      for (auto catches : fishery.catches_) cache << "catch[" << fishery.label_ << "][" << catches.first << "] ";
      for (auto actual_catches : fishery.actual_catches_) cache << "actual_catches[" << fishery.label_ << "][" << actual_catches.first << "] ";
    }

    if (process_profile_ == ProcessProfile::kAge) {
      for (auto& category : categories_) {
        for (unsigned i = 0; i < category.category_->data_.size(); ++i) {
          cache << "m_by_age[" << category.category_label_ << "][" << i + 1 << "] ";
        }
      }
    }
    cache << REPORT_EOL;
  }

  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    for (auto pressure : fishery.exploitation_by_year_) cache << AS_DOUBLE(pressure.second) << " ";
    for (auto pressure : fishery.uobs_by_year_) cache << AS_DOUBLE(pressure.second) << " ";
    for (auto catches : fishery.catches_) cache << AS_DOUBLE(catches.second) << " ";
    for (auto actual_catches : fishery.actual_catches_) cache << AS_DOUBLE(actual_catches.second) << " ";
  }

  if (process_profile_ == ProcessProfile::kAge) {
    for (auto& category : categories_) {
      for (unsigned i = 0; i < category.category_->data_.size(); ++i) {
        cache << AS_DOUBLE(category.m_values_[i]) << " ";
      }
    }
  }
  cache << REPORT_EOL;
}

/**
 * Check the categories in methods for removal obs
 */
bool MortalityInstantaneous::check_categories_in_methods_for_removal_obs(vector<string> methods, vector<string> category_labels) {
  LOG_TRACE();

  if (process_profile_ == ProcessProfile::kAge) {
    // Use the pre-built fishery_category_check_ map
    for (auto& method : methods) {
      if (fishery_category_check_.find(method) == fishery_category_check_.end())
        return false;
      for (auto& category : category_labels) {
        if (find(fishery_category_check_[method].begin(), fishery_category_check_[method].end(), category) == fishery_category_check_[method].end())
          return false;
      }
    }
  } else {
    // Length: iterate through fisheries directly
    for (unsigned fishery_index = 0; fishery_index < methods.size(); ++fishery_index) {
      unsigned categories_counter = 0;
      for (auto& fishery_iter : fisheries_) {
        auto& fishery = fishery_iter.second;
        if (fishery.label_ != methods[fishery_index])
          continue;
        for (unsigned category_index = 0; category_index < category_labels.size(); ++category_index) {
          for (auto& fishery_category : fishery_categories_) {
            if ((fishery_category.fishery_.label_ == fishery.label_) && (fishery_category.category_label_ == category_labels[category_index]))
              ++categories_counter;
          }
        }
        if (categories_counter != category_labels.size()) {
          LOG_FINEST() << "category counter = " << categories_counter << " categories supplies = " << category_labels.size();
          return false;
        }
      }
    }
  }

  return true;
}

/**
 * Check the years in methods for removal obs
 */
bool MortalityInstantaneous::check_years_in_methods_for_removal_obs(vector<unsigned> years, vector<string> methods) {
  LOG_TRACE();

  if (process_profile_ == ProcessProfile::kAge) {
    // Use the pre-built fishery_catch_ map
    for (auto& method : methods) {
      if (fishery_catch_.find(method) == fishery_catch_.end())
        return false;
      for (auto& year : years) {
        if (fishery_catch_[method].find(year) == fishery_catch_[method].end())
          return false;
        if (fishery_catch_[method][year] <= 0)
          return false;
      }
    }
  } else {
    // Length: iterate through fisheries directly
    for (unsigned fishery_index = 0; fishery_index < methods.size(); ++fishery_index) {
      for (auto& fishery_iter : fisheries_) {
        auto& fishery = fishery_iter.second;
        if (fishery.label_ != methods[fishery_index])
          continue;
        unsigned year_counter = 0;
        for (auto& catches : fishery.catches_) {
          if (find(years.begin(), years.end(), catches.first) != years.end()) {
            LOG_FINE() << "found year = " << catches.first << " with catch = " << catches.second;
            if (catches.second <= 0)
              return false;
            ++year_counter;
          }
        }
        if (year_counter != years.size())
          return false;
      }
    }
  }
  return true;
}

/**
 * Check the methods for removal obs
 */
bool MortalityInstantaneous::check_methods_for_removal_obs(vector<string> methods) {
  LOG_TRACE();
  unsigned method_counter = 0;
  for (unsigned fishery_index = 0; fishery_index < methods.size(); ++fishery_index) {
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      if (fishery.label_ == methods[fishery_index]) {
        ++method_counter;
        break;
      }
    }
  }
  return method_counter == methods.size();
}

/**
 * Get fishery index for catch at
 */
vector<unsigned> MortalityInstantaneous::get_fishery_ndx_for_catch_at(vector<string> fishery_labels) {
  LOG_TRACE();
  vector<unsigned> result;
  for (auto& label : fishery_labels) {
    if (fisheries_.find(label) != fisheries_.end())
      result.push_back(fisheries_[label].fishery_ndx_);
  }
  return result;
}

/**
 * Get category index for catch at
 */
vector<unsigned> MortalityInstantaneous::get_category_ndx_for_catch_at(vector<string> category_labels) {
  LOG_TRACE();
  vector<unsigned> result;
  for (auto& label : category_labels) {
    if (category_data_.find(label) != category_data_.end())
      result.push_back(category_data_[label]->category_ndx_);
  }
  return result;
}

/**
 * Get year index for catch at
 */
vector<unsigned> MortalityInstantaneous::get_year_ndx_for_catch_at(vector<unsigned> years) {
  LOG_TRACE();
  vector<unsigned> result;
  for (auto& year : years) {
    std::pair<bool, int> year_iter = findInVector(process_years_, year);
    if (year_iter.first)
      result.push_back(year_iter.second);
  }
  return result;
}

}  // namespace niwa::processes::common
