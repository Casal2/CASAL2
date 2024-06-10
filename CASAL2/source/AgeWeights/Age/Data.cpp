/**
 * @file Data.cpp
 * @author  C.Marsh
 * @date 16/11/2017
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "Data.h"

#include "../../Model/Managers.h"
#include "../../TimeSteps/Manager.h"
#include "../../Utilities/To.h"

// namespaces
namespace niwa {
namespace ageweights {

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
Data::Data(shared_ptr<Model> model) : AgeWeight(model) {
  data_table_ = new parameters::Table(PARAM_DATA);

  parameters_.BindTable(PARAM_DATA, data_table_, "", "");
  parameters_
      .Bind<string>(PARAM_EQUILIBRIUM_METHOD, &equilibrium_method_, "If used in an SSB calculation, what is the method to calculate equilibrium SSB", "", PARAM_TERMINAL_YEAR)
      ->set_allowed_values({PARAM_MEAN, PARAM_FIRST_YEAR, PARAM_TERMINAL_YEAR});
  parameters_.Bind<string>(PARAM_UNITS, &units_, "The units of measure (grams, kilograms (kgs), or tonnes)", "", PARAM_KGS)
      ->set_allowed_values({PARAM_KGS, PARAM_KILOGRAMS, PARAM_GRAMS, PARAM_TONNES});
}

/**
 * Destructor
 */
Data::~Data() {
  delete data_table_;
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */

void Data::DoBuild() {
  LOG_FINE() << "Building age weight block " << label_;

  if ((units_ == PARAM_TONNES) && (model_->base_weight_units() == PARAM_KGS || model_->base_weight_units() == PARAM_KILOGRAMS))
    unit_multipier_ = 1000;
  else if (units_ == PARAM_GRAMS && (model_->base_weight_units() == PARAM_KGS || model_->base_weight_units() == PARAM_KILOGRAMS))
    unit_multipier_ = 0.001;

  // Deal with base weight as tonnes
  if ((units_ == PARAM_KGS || units_ == PARAM_KILOGRAMS) && (model_->base_weight_units() == PARAM_TONNES))
    unit_multipier_ = 0.001;
  else if (units_ == PARAM_GRAMS && (model_->base_weight_units() == PARAM_TONNES))
    unit_multipier_ = 0.000001;

  // Deal with base weight as grams
  if ((units_ == PARAM_KGS || units_ == PARAM_KILOGRAMS) && (model_->base_weight_units() == PARAM_GRAMS))
    unit_multipier_ = 1000;
  else if (units_ == PARAM_TONNES && (model_->base_weight_units() == PARAM_GRAMS))
    unit_multipier_ = 1000000;

  if (!data_table_)
    LOG_FATAL_P(PARAM_DATA) << "could not find data table";
  if (model_->run_mode() == RunMode::kProjection)
    final_year_ = model_->projection_final_year();
  else
    final_year_ = model_->final_year();

  // basic validation
  const vector<string>& columns = data_table_->columns();
  if (columns.size() != model_->age_spread() + 1)
    LOG_FATAL_P(PARAM_DATA) << "column count (" << columns.size() << ") must be <year> <ages> for a total of " << model_->age_spread() + 1 << " columns";
  if (columns[0] != PARAM_YEAR)
    LOG_FATAL_P(PARAM_DATA) << "first column label must be 'year'. First column label was '" << columns[0] << "'";

  /*
   * Build our data_by_year map so we can fill the gaps
   * and use it in the model
   */

  for (unsigned i = 1; i < columns.size(); ++i) {
    age_.push_back(utilities::ToInline<string, unsigned>(columns[i]));
  }

  vector<vector<string>>& data = data_table_->data();
  vector<Double>          total_weight(model_->age_spread(), 0.0);
  Double                  number_of_years = 0.0;
  for (vector<string> row : data) {
    if (row.size() != columns.size())
      LOG_CODE_ERROR() << "row.size() != columns.size()";
    number_of_years += 1;
    if ((columns.size() - 1) != model_->age_spread())
      LOG_FATAL_P(PARAM_DATA) << "An age must be specified for every age in the model. " << columns.size() - 1 << " ages were specified, and there are " << model_->age_spread()
                              << " ages";

    unsigned year = utilities::ToInline<string, unsigned>(row[0]);
    // Check year is valid
    if (find(model_->years().begin(), model_->years().end(), year) == model_->years().end())
      LOG_WARNING() << "year " << year << " is not included in the model run years, so this age weight will not be used";

    LOG_FINE() << "Loading years = " << year;
    years_.push_back(year);
    for (unsigned i = 1; i < row.size(); ++i) {
      mean_data_by_year_and_age_[year][age_[i - 1]] = utilities::ToInline<string, Double>(row[i]) * unit_multipier_;
      data_by_year_[year].push_back(utilities::ToInline<string, Double>(row[i]) * unit_multipier_);
      total_weight[i - 1] += utilities::ToInline<string, Double>(row[i]) * unit_multipier_;
    }
  }

  // Check there are equal years as in the model
  if (model_->years().size() != years_.size())
    LOG_ERROR_P(PARAM_DATA) << "Specify the same number of years as the model has. " << years_.size() << " years were supplied, but there are " << model_->years().size()
                            << " years";

  LOG_FINEST() << "ages";
  for (auto age : age_) LOG_FINEST() << age;
  // Do some checks on the model age.

  // Build Equilibrium state
  if (equilibrium_method_ == PARAM_MEAN) {
    for (unsigned i = 0; i < model_->age_spread(); ++i) {
      Double total = 0.0;
      for (auto iter = data_by_year_.begin(); iter != data_by_year_.end(); ++iter) total += iter->second[i];
      initial_[age_[i]] = total / data_by_year_.size() * unit_multipier_;
    }
  } else if (equilibrium_method_ == PARAM_FIRST_YEAR) {
    auto first = data_by_year_.begin();
    for (unsigned i = 0; i < model_->age_spread(); ++i) {
      initial_[age_[i]] = first->second[i];
    }
  } else if (equilibrium_method_ == PARAM_TERMINAL_YEAR) {
    auto last = data_by_year_.rbegin();
    for (unsigned i = 0; i < model_->age_spread(); ++i) {
      initial_[age_[i]] = last->second[i];
    }
  }
  LOG_FINEST() << "initial weight at age";
  for (auto init : initial_) LOG_FINEST() << init.second;
}

/**
 * Get the mean weight of a single population by year and age (group)
 *
 * @param year The year
 * @param age The age (group)
 * @return mean weight
 */
Double Data::mean_weight_at_age_by_year(unsigned year, unsigned age) {
  if (model_->state() == State::kInitialise)
    return initial_[age];

  return mean_data_by_year_and_age_[year][age];
}

} /* namespace ageweights */
} /* namespace niwa */
