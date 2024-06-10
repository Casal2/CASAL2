/**
 * @file AgeingError.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/04/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// Headers
#include "AgeingError.h"

#include "../Model/Model.h"

// Namespaces
namespace niwa {

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
AgeingError::AgeingError(shared_ptr<Model> model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label of the ageing error", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of ageing error", "");
}

/**
 * Populate any parameters,
 * Validate that values are within expected ranges when bind<>() overloads cannot be used
 *
 * Note: all parameters are populated from configuration files
 */
void AgeingError::Validate() {
  parameters_.Populate(model_);

  min_age_    = model_->min_age();
  max_age_    = model_->max_age();
  plus_group_ = model_->age_plus();
  age_spread_ = model_->age_spread();

  DoValidate();
}

/**
 * Build an Empty misclassification matrix that will be populated by a child method
 */
void AgeingError::Build() {
  mis_matrix_.resize(age_spread_);
  for (unsigned i = 0; i < age_spread_; ++i) mis_matrix_[i].resize(age_spread_, 0);

  DoBuild();
}

} /* namespace niwa */
