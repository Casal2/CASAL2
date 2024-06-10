/**
 * @file Process.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/10/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "Process.h"

#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../Penalties/Manager.h"
#include "../../Utilities/Math.h"

// namespaces
namespace niwa {
namespace penalties {

/**
 * Default constructor
 */
Process::Process(shared_ptr<Model> model) : Penalty(model) {
  parameters_.Bind<Double>(PARAM_MULTIPLIER, &multiplier_, "The penalty multiplier", "", 1.0);
  parameters_.Bind<bool>(PARAM_LOG_SCALE, &log_scale_, "Indicates if the sums of squares is calculated on the log scale", "", false);

  has_score_ = false;
}

/**
 * Trigger the penalty.
 * Basic value for the trigger will be: (value_1 - value_2)^2 * multiplier
 * logscale is: (log(value_1) - log(value_2))^2 * multiplier
 *
 * @param value_1 The first value to use in equation
 * @param value_2 The second value to use in equation
 */
void Process::Trigger(Double value_1, Double value_2) {
  if (log_scale_) {
    value_1 = log(utilities::math::ZeroFun(value_1));
    value_2 = log(utilities::math::ZeroFun(value_2));
  }
  //Double value = (value_1 - value_2) * (value_1 - value_2) * multiplier_;
  score_ += (value_1 - value_2) * (value_1 - value_2) * multiplier_;
}


} /* namespace penalties */
} /* namespace niwa */
