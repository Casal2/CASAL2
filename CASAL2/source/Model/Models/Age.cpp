/**
 * @file Age.cpp
 * @author  Scott Rasmussen (scott@zaita.com)
 * @date 20/09/2019
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 */

// headers
#include "Age.h"

// namespaces
namespace niwa::model {

/**
 * @brief Construct a new Age:: Age object
 *
 */
Age::Age() {
  partition_type_ = PartitionType::kAge;

  parameters_.Bind<unsigned>(PARAM_START_YEAR, &start_year_, "Define the first year of the model, immediately following initialisation",
                             R"(Defines the first year of the model, $\ge 1$, e.g. 1990)");
  parameters_.Bind<unsigned>(PARAM_FINAL_YEAR, &final_year_, "Define the final year of the model, excluding years in the projection period",
                             "Defines the last year of the model, i.e., the model is run from start_year to final_year");
  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "Minimum age of individuals in the population", R"($0 \le$ age\textlow{min} $\le$ age\textlow{max})", 0)->set_partition_type(PartitionType::kAge);
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "Maximum age of individuals in the population", R"($0 \le$ age\textlow{min} $\le$ age\textlow{max})", 0)->set_partition_type(PartitionType::kAge);
  parameters_.Bind<bool>(PARAM_AGE_PLUS, &age_plus_, "Define the oldest age or extra length midpoint (plus group size) as a plus group", "true, false", true)->set_partition_type(PartitionType::kAge);
  parameters_.Bind<string>(PARAM_INITIALISATION_PHASES, &initialisation_phases_, "Define the labels of the phases of the initialisation",
                           R"(A list of valid labels defined by \texttt{@initialisation_phase})", true);
  parameters_.Bind<string>(PARAM_TIME_STEPS, &time_steps_, "Define the labels of the time steps, in the order that they are applied, to form the annual cycle",
                           R"(A list of valid labels defined by \texttt{@time_step})");
  parameters_.Bind<unsigned>(PARAM_PROJECTION_FINAL_YEAR, &projection_final_year_, "Define the final year of the model when running projections", "A value greater than final_year",
                             0);
  parameters_.Bind<double>(PARAM_LENGTH_BINS, &model_length_bins_, "The minimum length in each length bin", R"($0 \le$ length\textlow{min} $\le$ length\textlow{max})", true);
  parameters_.Bind<bool>(PARAM_LENGTH_PLUS, &length_plus_, "Specify whether there is a length plus group or not", "true, false", true);
  parameters_.Bind<double>(PARAM_LENGTH_PLUS_GROUP, &length_plus_group_, "Mean length of length plus group", R"(length\textlow{max} $<$ length_plus_group)", 0)->set_partition_type(PartitionType::kLength);
}

/**
 * @brief Validate our AgeModel
 *
 */
void Age::DoValidate() {
  if (min_age_ > max_age_)
    LOG_ERROR_P(PARAM_MIN_AGE) << " (" << min_age_ << ") has been defined as greater than max_age (" << max_age_ << ")";
  if (start_year_ < 1000)
    LOG_ERROR_P(PARAM_START_YEAR) << " (" << start_year_ << ") cannot be less than 1000";
  if (start_year_ > final_year_)
    LOG_ERROR_P(PARAM_FINAL_YEAR) << " (" << final_year_ << ") cannot be less than start_year (" << start_year_ << ")";

  if (run_mode_ == RunMode::kProjection) {
    if (projection_final_year_ <= final_year_) {
      LOG_ERROR_P(PARAM_PROJECTION_FINAL_YEAR) << "(" << projection_final_year_ << ") cannot be less than or equal to final_year (" << final_year_ << ")";
    }
  }

  number_of_model_length_bins_ = length_plus_ == true ? model_length_bins_.size() : model_length_bins_.size() - 1;
  LOG_FINE() << "number of length bins supplied = " << model_length_bins_.size() << " number length bins " << number_of_model_length_bins_;

}

}  // namespace niwa::model
