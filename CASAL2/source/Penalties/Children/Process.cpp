/**
 * @file Process.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Process.h"

#include "Model/Model.h"
#include "Model/Managers.h"
#include "Penalties/Manager.h"
#include "Utilities/DoubleCompare.h"

// namespaces
namespace niwa {
namespace penalties {

/**
 * Default Constructor
 */
Process::Process(Model* model) : Penalty(model) {
  parameters_.Bind<Double>(PARAM_MULTIPLIER, &multiplier_, "Multiply the penalty by this factor", "", 1.0);
  parameters_.Bind<bool>(PARAM_LOG_SCALE, &log_scale_, "Should sums of squares be calculated on the log scale?", "", false);

  has_score_ = false;
}

/**
 * Trigger our penalty.
 * Basic value for the trigger will be: (value_1 - value_2)^2 * multiplier
 * logscale is: (log(value_1) - log(value_2))^2 * multiplier
 *
 * @param source_label The label for the source of the trigger
 * @param value_1 The first value to use in equation
 * @param value_2 The second valud to use in equatin
 */
void Process::Trigger(const string& source_label, Double value_1, Double value_2) {

  if (log_scale_) {
    value_1 = log(utilities::doublecompare::ZeroFun(value_1));
    value_2 = log(utilities::doublecompare::ZeroFun(value_2));
  }

  string name  = label_ + "(" + source_label + ")";
  Double value = (value_1 - value_2) * (value_1 - value_2) * multiplier_;
  model_->managers().penalty()->FlagPenalty(name, value);
}

} /* namespace penalties */
} /* namespace niwa */
