/**
 * @file AgeingError.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// Headers
#include "AgeingError.h"

#include "Model/Model.h"

// Namespaces
namespace niwa {

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
AgeingError::AgeingError() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "");
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void AgeingError::Validate() {
  parameters_.Populate();

  ModelPtr model = Model::Instance();
  min_age_    = model->min_age();
  max_age_    = model->max_age();
  age_plus_   = model->age_plus();
  age_spread_ = model->age_spread();

  DoValidate();
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void AgeingError::Build() {
  mis_matrix_.resize(age_spread_);
  for (unsigned i = 0; i < age_spread_; ++i)
    mis_matrix_[i].resize(age_spread_, 0);

  DoBuild();
}

} /* namespace niwa */
