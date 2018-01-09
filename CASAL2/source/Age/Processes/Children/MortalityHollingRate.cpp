/**
 * @file MortalityHollingRate.cpp
 * @author C. Marsh
 * @github https://github.com/Zaita
 * @date 31/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "MortalityHollingRate.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>

#include "Common/Categories/Categories.h"
#include "Common/Model/Managers.h"
#include "Common/Penalties/Manager.h"
#include "Common/Selectivities/Manager.h"
#include "Common/TimeSteps/TimeStep.h"
#include "Common/TimeSteps/Manager.h"
#include "Common/Utilities/DoubleCompare.h"
#include "Common/Utilities/To.h"


// namespaces
namespace niwa {
namespace age {
namespace processes {
namespace dc = niwa::utilities::doublecompare;
/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g profiling, yields, projections etc)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate Latex for the documentation.
 */
MortalityHollingRate::MortalityHollingRate(Model* model)
  : Process(model),
    prey_partition_(model),
    predator_partition_(model) {
  process_type_ = ProcessType::kMortality;
  partition_structure_ = PartitionType::kAge;


  predator_selectivitie_table_ = new parameters::Table(PARAM_PREDATOR_SELECTIVITIES);
  prey_selectivitie_table_ = new parameters::Table(PARAM_PREY_SELECTIVITIES);

  parameters_.BindTable(PARAM_PREDATOR_SELECTIVITIES_BY_YEAR, predator_selectivitie_table_, "Table of predator selectivities by year and age", "", true, true);
  parameters_.BindTable(PARAM_PREY_SELECTIVITIES_BY_YEAR, prey_selectivitie_table_, "Table of prey selectivities by year and age", "", true, true);
  parameters_.Bind<string>(PARAM_PREY_CATEGORIES, &prey_category_labels_, "Prey Categories labels", "");
  parameters_.Bind<string>(PARAM_PREDATOR_CATEGORIES, &predator_category_labels_, "Predator Categories labels", "");
  parameters_.Bind<bool>(PARAM_IS_ABUNDANCE, &is_abundance_, "Is vulnerable amount of prey and predator an abundance [true] or biomass [false]", "", true);
  parameters_.Bind<Double>(PARAM_A, &a_, "parameter a", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_B, &b_, "parameter b", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_X, &x_, "This parameter controls the type of functional form, Holling function type 2 (x=2) or 3 (x=3), or generalised (Michaelis Menten, x>=1)", "")->set_lower_bound(1.0);
  parameters_.Bind<Double>(PARAM_U_MAX, &u_max_, "Maximum exploitation rate ($Umax$)", "")->set_range(0.0, 1.0);
  parameters_.Bind<string>(PARAM_PREY_SELECTIVITIES, &prey_selectivity_labels_, "Selectivities for prey categories", "", true);
  parameters_.Bind<string>(PARAM_PREDATOR_SELECTIVITIES, &predator_selectivity_labels_, "Selectivities for predator categories", "", true);
  parameters_.Bind<string>(PARAM_PENALTY, &  penalty_label_, "Label of penalty to be applied", "","");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years in which to apply the mortality process", "");

  RegisterAsAddressable(PARAM_A, &a_);
  RegisterAsAddressable(PARAM_B, &b_);
  RegisterAsAddressable(PARAM_X, &x_);
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */

void MortalityHollingRate::DoValidate() {
  LOG_TRACE();

  /**
   * Check All the categories are valid
   */
  for (const string& label : prey_category_labels_) {
    if (!model_->categories()->IsValid(label))
      LOG_ERROR_P(PARAM_CATEGORIES) << ": category " << label << " does not exist. Have you defined it?";
  }

  for (const string& label : predator_category_labels_) {
    if (!model_->categories()->IsValid(label))
      LOG_ERROR_P(PARAM_CATEGORIES) << ": category " << label << " does not exist. Have you defined it?";
  }
  LOG_TRACE();
  LOG_FINEST() << "prey = " << parameters_.Get(PARAM_PREY_SELECTIVITIES)->has_been_defined();
  LOG_FINEST() << "prey by year = " << parameters_.GetTable(PARAM_PREY_SELECTIVITIES_BY_YEAR)->has_been_defined();
  LOG_TRACE();
  // Check how the user has selected selectivities, this is an unusal process where we allow an alternative way to parameterise the selectivity
  if (!parameters_.Get(PARAM_PREY_SELECTIVITIES)->has_been_defined() && !parameters_.GetTable(PARAM_PREY_SELECTIVITIES_BY_YEAR)->has_been_defined()) {
    LOG_FATAL_P(PARAM_LABEL) << "you need to supply either '" << PARAM_PREY_SELECTIVITIES << "' or '" << PARAM_PREY_SELECTIVITIES_BY_YEAR << "', for this process to work";
    LOG_TRACE();
  }
  if (parameters_.Get(PARAM_PREY_SELECTIVITIES)->has_been_defined() & parameters_.GetTable(PARAM_PREY_SELECTIVITIES_BY_YEAR)->has_been_defined())
    LOG_FATAL_P(PARAM_LABEL) << "you cannot supply both '" << PARAM_PREY_SELECTIVITIES << "' or '" << PARAM_PREY_SELECTIVITIES_BY_YEAR << "', for this process to work. Please choose one selectivity method";
  LOG_TRACE();
  if (!parameters_.Get(PARAM_PREDATOR_SELECTIVITIES)->has_been_defined() & !parameters_.GetTable(PARAM_PREDATOR_SELECTIVITIES_BY_YEAR)->has_been_defined())
    LOG_FATAL_P(PARAM_LABEL) << "you need to supply either '" << PARAM_PREDATOR_SELECTIVITIES << "' or '" << PARAM_PREDATOR_SELECTIVITIES_BY_YEAR << "', for this process to work";
  LOG_TRACE();
  if (parameters_.Get(PARAM_PREDATOR_SELECTIVITIES)->has_been_defined() & parameters_.GetTable(PARAM_PREDATOR_SELECTIVITIES_BY_YEAR)->has_been_defined())
    LOG_FATAL_P(PARAM_LABEL) << "you cannot supply both '" << PARAM_PREDATOR_SELECTIVITIES << "' or '" << PARAM_PREDATOR_SELECTIVITIES_BY_YEAR << "', for this process to work. Please choose one selectivity method";

  LOG_TRACE();
  if (parameters_.Get(PARAM_PREY_SELECTIVITIES)->has_been_defined()) {
    if (prey_category_labels_.size() != prey_selectivity_labels_.size())
      LOG_ERROR_P(PARAM_PREY_CATEGORIES) << ": You provided (" << prey_selectivity_labels_.size() << ") prey selectivities but we have "
          << prey_category_labels_.size() << " prey catregories";
  }
  LOG_TRACE();
  if (parameters_.Get(PARAM_PREDATOR_SELECTIVITIES)->has_been_defined()) {
    if (predator_category_labels_.size() != predator_selectivity_labels_.size())
      LOG_ERROR_P(PARAM_PREDATOR_CATEGORIES) << ": You provided (" << predator_selectivity_labels_.size() << ") prey selectivities but we have "
          << predator_category_labels_.size() << " prey catregories";
  }
  LOG_TRACE();
  // Populate maps from tables if user has defined them
  if (parameters_.GetTable(PARAM_PREY_SELECTIVITIES_BY_YEAR)->has_been_defined()) {
    prey_selectivity_by_year_supplied_ = true;
    auto columns = prey_selectivitie_table_->columns();
    if (columns.size() != (model_->age_spread() + 1) )
      LOG_FATAL_P(PARAM_PREY_SELECTIVITIES_BY_YEAR) << "you need to specify a column for each model age plus a year column, which should = " << (model_->age_spread() + 1) << " you supplied " << columns.size() << " please address this";
    if (std::find(columns.begin(), columns.end(), PARAM_YEAR) == columns.end())
      LOG_ERROR_P(PARAM_PREY_SELECTIVITIES_BY_YEAR) << "Cannot find the column " << PARAM_YEAR << ", this column is needed, for casal2 to run this process. Please add it =)";

    unsigned year_index = std::find(columns.begin(), columns.end(), PARAM_YEAR) - columns.begin();
    LOG_FINEST() << "year column index for prey selectivity table is: " << year_index;

    if (year_index != 0) {
      LOG_FATAL_P(PARAM_PREY_SELECTIVITIES_BY_YEAR) << "expected the 'year' column to be the first column, please make sure the first column are years";
    }
    vector<unsigned> ages;
    for (unsigned age = 1; age < columns.size(); ++age) {
      unsigned temp_age;
      if(!utilities::To<string, unsigned>(columns[age], temp_age))
        LOG_FATAL_P(PARAM_PREY_SELECTIVITIES_BY_YEAR) << "age " << columns[age] << " could not be converted to an unsigned, please check this value";
      ages.push_back(temp_age);
    }
    // Check the ages are ascending order
    for (unsigned age = 0; age < (ages.size() - 1); ++age) {
      if (ages[age] + 1 != ages[age + 1])
        LOG_FATAL_P(PARAM_PREY_SELECTIVITIES_BY_YEAR) << "expected consecutive ascending ages in the columns. The following consecutive numbers are not incremental increase which is what we expect '" << ages[age] << "', " << ages[age + 1];
    }


    auto model_years = model_->years();
    auto rows = prey_selectivitie_table_->data();
    for (auto row : rows) {
      unsigned year = 0;
      if (!utilities::To<string, unsigned>(row[year_index], year))
        LOG_ERROR_P(PARAM_PREY_SELECTIVITIES_BY_YEAR) << "year value " << row[year_index] << " is not numeric.";
      if (std::find(model_years.begin(), model_years.end(), year) == model_years.end())
        LOG_ERROR_P(PARAM_PREY_SELECTIVITIES_BY_YEAR) << "year " << year << " is not a valid year in this model";
      for (unsigned i = 1; i < row.size(); ++i) {
        Double value = 0.0;
        if (!utilities::To<string, Double>(row[i], value))
          LOG_ERROR_P(PARAM_PREY_SELECTIVITIES_BY_YEAR) << "value " << row[i] << " for age " << ages[i + 1] << " is not numeric";
        prey_selectivity_by_year_[year].push_back(value);
      }
    }
  }
  LOG_TRACE();
  // Predator table
  if (parameters_.GetTable(PARAM_PREDATOR_SELECTIVITIES_BY_YEAR)->has_been_defined()) {
    predator_selectivity_by_year_supplied_ = true;
    auto columns = predator_selectivitie_table_->columns();
    if (columns.size() != (model_->age_spread() + 1) )
      LOG_FATAL_P(PARAM_PREDATOR_SELECTIVITIES_BY_YEAR) << "you need to specify a column for each model age plus a year column, which should = " << (model_->age_spread() + 1) << " you supplied " << columns.size() << " please address this";
    if (std::find(columns.begin(), columns.end(), PARAM_YEAR) == columns.end())
      LOG_ERROR_P(PARAM_PREDATOR_SELECTIVITIES_BY_YEAR) << "Cannot find the column " << PARAM_YEAR << ", this column is needed, for casal2 to run this process. Please add it =)";

    unsigned year_index = std::find(columns.begin(), columns.end(), PARAM_YEAR) - columns.begin();
    LOG_FINEST() << "year column index for predator selectivity table is: " << year_index;

    if (year_index != 0) {
      LOG_FATAL_P(PARAM_PREDATOR_SELECTIVITIES_BY_YEAR) << "expected the 'year' column to be the first column, please make sure the first column are years";
    }
    vector<unsigned> ages;
    for (unsigned age = 1; age < columns.size(); ++age) {
      unsigned temp_age;
      if(!utilities::To<string, unsigned>(columns[age], temp_age))
        LOG_FATAL_P(PARAM_PREDATOR_SELECTIVITIES_BY_YEAR) << "age " << columns[age] << " could not be converted to an unsigned, please check this value";
      ages.push_back(temp_age);
    }
    // Check the ages are ascending order
    for (unsigned age = 0; age < (ages.size() - 1); ++age) {
      if (ages[age] + 1 != ages[age + 1])
        LOG_FATAL_P(PARAM_PREDATOR_SELECTIVITIES_BY_YEAR) << "expected consecutive ascending ages in the columns. The following consecutive numbers are not incremental increase which is what we expect '" << ages[age] << "', " << ages[age + 1];
    }


    auto model_years = model_->years();
    auto rows = predator_selectivitie_table_->data();
    for (auto row : rows) {
      unsigned year = 0;
      if (!utilities::To<string, unsigned>(row[year_index], year))
        LOG_ERROR_P(PARAM_PREDATOR_SELECTIVITIES_BY_YEAR) << "year value " << row[year_index] << " is not numeric.";
      if (std::find(model_years.begin(), model_years.end(), year) == model_years.end())
        LOG_ERROR_P(PARAM_PREDATOR_SELECTIVITIES_BY_YEAR) << "year " << year << " is not a valid year in this model";
      for (unsigned i = 1; i < row.size(); ++i) {
        Double value = 0.0;
        if (!utilities::To<string, Double>(row[i], value))
          LOG_ERROR_P(PARAM_PREDATOR_SELECTIVITIES_BY_YEAR) << "value " << row[i] << " for age " << ages[i + 1] << " is not numeric";
        predator_selectivity_by_year_[year].push_back(value);
      }
    }
  }
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 *
 * Validate any parameters that require information from other objects
 * in the system
 */
void MortalityHollingRate::DoBuild() {
  LOG_TRACE();
  prey_partition_.Init(prey_category_labels_);
  predator_partition_.Init(predator_category_labels_);
  /**
   * Assign the selectivity, penalty and time step index to each fisher data object
   */
  unsigned category_offset = 0;
  if (!prey_selectivity_by_year_supplied_) {
    unsigned category_offset = 0;
    for (string selectivity : prey_selectivity_labels_) {
      prey_selectivities_.push_back(model_->managers().selectivity()->GetSelectivity(selectivity));
      if (!prey_selectivities_[category_offset])
        LOG_ERROR_P(PARAM_PREY_SELECTIVITIES) << "selectivity " << selectivity << " does not exist. Have you defined it?";
      ++category_offset;
    }
  }

  if (!predator_selectivity_by_year_supplied_) {
    category_offset = 0;
    for (string selectivity : predator_selectivity_labels_) {
      predator_selectivities_.push_back(model_->managers().selectivity()->GetSelectivity(selectivity));
      if (!predator_selectivities_[category_offset])
        LOG_ERROR_P(PARAM_PREDATOR_SELECTIVITIES) << "selectivity " << selectivity << " does not exist. Have you defined it?";
      ++category_offset;
    }
  }

  if (penalty_label_ != "none") {
    penalty_ = model_->managers().penalty()->GetProcessPenalty(penalty_label_);
    if (!penalty_)
      LOG_ERROR_P(PARAM_PENALTY) << ": penalty " << penalty_label_ << " does not exist. Have you defined it?";
  }
  // Pre allocate memory
  prey_vulnerability_by_year_.reserve(years_.size());
  predator_vulnerability_by_year_.reserve(years_.size());
  prey_mortality_by_year_.reserve(years_.size());
}

/**
 * Execute this process
 */
void MortalityHollingRate::DoExecute() {
  LOG_TRACE();
  // Check if we are executing this process in current year
  if (std::find(years_.begin(), years_.end(), model_->current_year()) != years_.end()) {
    unsigned time_step_index = model_->managers().time_step()->current_time_step();

    /**
     * Loop for prey each category, calculate vulnerable abundance
     */
    unsigned current_year = model_->current_year();
    Double Mortality = 0;
    Double SumMortality = 0;
    Double Vulnerable = 0;
    Double PredatorVulnerable = 0;
    unsigned prey_offset = 0;

    for (auto prey_categories : prey_partition_) {
      if (!is_abundance_) {
        if (prey_selectivity_by_year_supplied_) {
          for (unsigned i = 0; i < prey_categories->data_.size(); ++i)
            Vulnerable += prey_categories->data_[i] * prey_selectivity_by_year_[current_year][i] * prey_categories->mean_weight_by_time_step_age_[time_step_index][prey_categories->min_age_ + i];
        } else {
          for (unsigned i = 0; i < prey_categories->data_.size(); ++i)
          Vulnerable += prey_categories->data_[i] * prey_selectivities_[prey_offset]->GetAgeResult(prey_categories->min_age_ + i, prey_categories->age_length_) * prey_categories->mean_weight_by_time_step_age_[time_step_index][prey_categories->min_age_ + i];
        }
      } else {
        if (prey_selectivity_by_year_supplied_) {
          for (unsigned i = 0; i < prey_categories->data_.size(); ++i)
            Vulnerable += prey_categories->data_[i] *  prey_selectivity_by_year_[current_year][i];
        } else {
          for (unsigned i = 0; i < prey_categories->data_.size(); ++i)
             Vulnerable += prey_categories->data_[i] * prey_selectivities_[prey_offset]->GetAgeResult(prey_categories->min_age_ + i, prey_categories->age_length_);
        }
      }
      ++prey_offset;
    }
    LOG_FINE() << "Vulnerable biomass of all prey categories = "  << Vulnerable;
    prey_vulnerability_by_year_.push_back(Vulnerable);
    /**
     * Loop for predator each category, calculate vulnerable predator abundance
     */
    unsigned predator_offset = 0;
    for (auto predator_categories : predator_partition_) {
      if (!is_abundance_) {
        if (predator_selectivity_by_year_supplied_) {
          for (unsigned i = 0; i < predator_categories->data_.size(); ++i)
            PredatorVulnerable += predator_categories->data_[i] * predator_selectivity_by_year_[current_year][i] * predator_categories->mean_weight_by_time_step_age_[time_step_index][predator_categories->min_age_ + i];
        } else {
          for (unsigned i = 0; i < predator_categories->data_.size(); ++i)
            PredatorVulnerable += predator_categories->data_[i] * predator_selectivities_[predator_offset]->GetAgeResult(predator_categories->min_age_ + i, predator_categories->age_length_) * predator_categories->mean_weight_by_time_step_age_[time_step_index][predator_categories->min_age_ + i];
        }
      } else {
        if (predator_selectivity_by_year_supplied_) {
          for (unsigned i = 0; i < predator_categories->data_.size(); ++i)
            PredatorVulnerable += predator_categories->data_[i] * predator_selectivity_by_year_[current_year][i];
        } else {
          for (unsigned i = 0; i < predator_categories->data_.size(); ++i)
            PredatorVulnerable += predator_categories->data_[i] * predator_selectivities_[predator_offset]->GetAgeResult(predator_categories->min_age_ + i, predator_categories->age_length_);
        }
      }
      ++predator_offset;
    }
    LOG_FINE() << "Vulnerable biomass of all predator categories = "  << PredatorVulnerable;
    predator_vulnerability_by_year_.push_back(PredatorVulnerable);

    // Holling function type 2 (x=1) or 3 (x=2), or generalised (Michaelis Menten)
    Mortality = PredatorVulnerable * (a_ * pow(Vulnerable, (x_ - 1.0))) / (b_ + pow(Vulnerable, (x_ - 1.0)));
    prey_mortality_by_year_.push_back(Mortality);
    // Work out exploitation rate to remove (catch/vulnerable Abundance)
    Double Exploitation = Mortality / dc::ZeroFun(Vulnerable, ZERO);

    if (Exploitation > u_max_) {
      LOG_FINE() << "Exloitation rate larger than u_max = " << Exploitation << " this is rescaled to u_max = " << u_max_;
      Exploitation = u_max_;

      if (penalty_) // Throw Penalty
        penalty_->Trigger(penalty_label_, Mortality, (Vulnerable * u_max_));

    } else if (Exploitation < 0.0)
        Exploitation = 0.0;

    prey_offset = 0;
    // Loop Through Prey Categories & remove number based on calcuated exploitation rate
    for (auto prey_categories : prey_partition_) {
      for (unsigned i = 0; i < prey_categories->data_.size(); ++i) {
        Double Current = 0.0;
        // Get Amount to remove
        if (prey_selectivity_by_year_supplied_) {
          Current = prey_categories->data_[i] * prey_selectivity_by_year_[current_year][i] * Exploitation;
        } else  {
          Current = prey_categories->data_[i] * prey_selectivities_[prey_offset]->GetAgeResult(prey_categories->min_age_ + i, prey_categories->age_length_) * Exploitation;
        }
          // If is Zero, Cont
        if (Current <= 0.0)
          continue;
        LOG_FINEST() << "Number of individuals removed from this process = " << Current;
        // remove abundance
        prey_categories->data_[i] -= Current;
        SumMortality += Current;
      }
      ++prey_offset;
    }
  } //if (std::find(years_.begin(), years_.end(), model_->current_year()) != years_.end()) {
}

/*
 * @fun FillReportCache
 * @description A method for reporting process information
 * @param cache a cache object to print to
*/
void MortalityHollingRate::FillReportCache(ostringstream& cache) {
  // This one is niggly because we need to iterate over each year and time step to print the right information so we don't
  cache << "prey_vulnerable: ";
  for (auto prey_vulnerable : prey_vulnerability_by_year_)
    cache << prey_vulnerable << " ";
  cache << "\n";
  cache << "predator_vulnerable: ";
  for (auto pred_vulnerable : predator_vulnerability_by_year_)
    cache << pred_vulnerable << " ";
  cache << "\n";
  cache << "Prey_Mortality: ";
  for (auto prey_mort : prey_mortality_by_year_)
    cache << prey_mort << " ";
}

/*
 * @fun FillTabularReportCache
 * @description A method for reporting tabular process information
 * @param cache a cache object to print to
 * @param first_run whether to print the header
 *
*/
void MortalityHollingRate::FillTabularReportCache(ostringstream& cache, bool first_run) {
/*  if (first_run) {
    // print header
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      for (auto pressure : fishery.exploitation_by_year_)
        cache << "fishing_pressure[" << fishery.label_ << "][" << pressure.first << "] ";
      for (auto catches : fishery.catches_)
        cache << "catch[" << fishery.label_ << "][" << catches.first << "] ";
      for (auto actual_catches : fishery.actual_catches_)
        cache << "actual_catches[" << fishery.label_ << "][" << actual_catches.first << "] ";
    }
    cache << "\n";
  }
  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    for (auto pressure : fishery.exploitation_by_year_)
      cache << pressure.second << " ";
    for (auto catches : fishery.catches_)
      cache << catches.second << " ";
    for (auto actual_catches : fishery.actual_catches_)
      cache <<  actual_catches.second << " ";
  }*/
}
} /* namespace processes */
} /* namespace age */
} /* namespace niwa */
