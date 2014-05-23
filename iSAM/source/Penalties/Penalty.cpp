/**
 * @file Penalty.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Penalty.h"

#include "Penalties/Manager.h"
#include "Utilities/DoubleCompare.h"

// Namespaces
namespace isam {

/**
 * Default Constructor
 */
Penalty::Penalty() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<double>(PARAM_MULTIPLIER, &multiplier_, "Multiplier", "", 1.0);
  parameters_.Bind<bool>(PARAM_LOG_SCALE, &log_scale_, "Log scale", "", false);
}

/**
 * Validate our penalty parameters
 */
void Penalty::Validate() {
  parameters_.Populate();
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
void Penalty::Trigger(const string& source_label, Double value_1, Double value_2) {

  if (log_scale_) {
    value_1 = log(utilities::doublecompare::ZeroFun(value_1));
    value_2 = log(utilities::doublecompare::ZeroFun(value_2));
  }

  string name  = label_ + "(" + source_label + ")";
  Double value = (value_1 - value_2) * (value_1 - value_2) * multiplier_;
  penalties::Manager::Instance().FlagPenalty(name, value);
}
} /* namespace isam */
