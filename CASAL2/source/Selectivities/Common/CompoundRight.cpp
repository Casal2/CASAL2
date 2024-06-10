/**
 * @file CompoundRight.cpp
 * @file CompoundRight.h
 * @author  C.Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "CompoundRight.h"

#include <boost/math/distributions/lognormal.hpp>
#include <cmath>

#include "../../AgeLengths/AgeLength.h"
#include "../../Model/Model.h"
#include "../../TimeSteps/Manager.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Default constructor
 */
CompoundRight::CompoundRight(shared_ptr<Model> model) : Selectivity(model) {


  parameters_.Bind<Double>(PARAM_A50, &a50_, "The mean (mu)", "");
  parameters_.Bind<Double>(PARAM_ATO95, &a_to95_, "The sigma L parameter", "")->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_A_MIN, &amin_, "The sigma R parameter", "")->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_LEFT_MEAN, &leftmu_, "The maximum value of the selectivity", "", 1.0)->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_TO_RIGHT_MEAN, &to_rightmu_, "The maximum value of the selectivity", "", 1.0)->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_SIGMA, &sd_, "The maximum value of the selectivity", "", 1.0)->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_ALPHA, &alpha_, "The maximum value of the selectivity", "", 1.0)->set_lower_bound(0.0, false);


  RegisterAsAddressable(PARAM_A50, &a50_);
  RegisterAsAddressable(PARAM_ATO95, &a_to95_);
  RegisterAsAddressable(PARAM_A_MIN, &amin_);
  RegisterAsAddressable(PARAM_LEFT_MEAN, &leftmu_);
  RegisterAsAddressable(PARAM_TO_RIGHT_MEAN, &to_rightmu_);
  RegisterAsAddressable(PARAM_SIGMA, &sd_);
  allowed_length_based_in_age_based_model_ = true;
}

/**
 * Validate this selectivity. This will load the
 * values that were passed in from the configuration
 * file and assign them to the local variables.
 *
 * Then do some basic checks on the local
 * variables to ensure they are within the business
 * rules for the model.
 */
void CompoundRight::DoValidate() {

}
/**
 * The core function
 */
Double CompoundRight::get_value(Double value) {
  Double Logistic = (1.0-amin_)/(1.0+pow(19.0,(a50_-value)/a_to95_)) + amin_;
  Double Left = 1.0 / (1.0+pow(19.0,(leftmu_ + to_rightmu_-value) / sd_));
  return Logistic * Left;
}
/**
 * The core function
 */
Double CompoundRight::get_value(unsigned value) {
  Double Logistic = (1.0-amin_)/(1.0+pow(19.0,(a50_-value)/a_to95_)) + amin_;
  Double Left = 1.0 / (1.0+pow(19.0,(leftmu_ + to_rightmu_-value) / sd_));
  return Logistic * Left;
}

} /* namespace selectivities */
} /* namespace niwa */
