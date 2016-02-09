/**
 * @file RandomWalk.cpp
 * @author Craig Marsh
 * @github https://github.com/Zaita
 * @date 2/02/2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "RandomWalk.h"

#include "Utilities/Map.h"
#include "Utilities/RandomNumberGenerator.h"
#include "Model/Objects.h"

// namespaces
namespace niwa {
namespace timevarying {

/**
 * Default constructor
 */
RandomWalk::RandomWalk(Model* model) : TimeVarying(model) {
  parameters_.Bind<Double>(PARAM_MEAN, &mu_, "Mean", "", 0);
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "Standard deviation", "", 1);
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_, "distribution", "", PARAM_NORMAL);

  RegisterAsEstimable(PARAM_MEAN, &mu_);
  RegisterAsEstimable(PARAM_SIGMA, &sigma_);
}

/**
 *
 */
void RandomWalk::DoValidate() {
  if (distribution_ != PARAM_NORMAL)
    LOG_ERROR() << "Random Walk can only can draw from a normal distribution currently";
}

/**
 *
 */
void RandomWalk::DoBuild() {

}

/**
 *
 */
void RandomWalk::DoReset() {

}

/**
 *
 */
void RandomWalk::DoUpdate() {
  string error = "";
  value_ = model_->objects().GetEstimable(parameter_, error);
  LOG_FINEST() << "value = " << (*value_);

  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  Double deviate = rng.normal(AS_DOUBLE(mu_), AS_DOUBLE(sigma_));
  (*value_) += deviate;

  LOG_FINEST() << "value after deviate of " << deviate << " = " << (*value_);

  LOG_FINE() << "Setting Value to: " << (*value_);
  (this->*update_function_)(*value_);
}

} /* namespace timevarying */
} /* namespace niwa */
