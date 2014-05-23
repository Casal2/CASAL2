/**
 * @file AgeingError.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "AgeingError.h"

#include "Model/Model.h"

// Namespaces
namespace isam {

/**
 * Default constructor
 */
AgeingError::AgeingError() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "");
}

/**
 * Destructor
 */
AgeingError::~AgeingError() {
}

/**
 * Validate the parameters
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
 * Build our matrix then call the child DoBuild method
 */
void AgeingError::Build() {
  mis_matrix_.resize(age_spread_);
  for (unsigned i = 0; i < age_spread_; ++i)
    mis_matrix_[i].resize(age_spread_, 0);

  DoBuild();
}

} /* namespace isam */
