/**
 * @file Factory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 22/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "Observations/DataWeights/Children/Dispersion.h"
#include "Observations/DataWeights/Children/Francis.h"
#include "Observations/DataWeights/Children/Multiplicative.h"
#include "Observations/DataWeights/Children/None.h"

// namespaces
namespace niwa {
namespace observations {
namespace dataweights {

/**
 * Create method
 */
DataWeightPtr Factory::Create(const std::string& type) {
  DataWeightPtr result;

  if (type == PARAM_DISPERSION)
    result = DataWeightPtr(new Dispersion());
  else if (type == PARAM_FRANCIS)
    result = DataWeightPtr(new Francis());
  else if (type == PARAM_MULTIPLICATIVE)
    result = DataWeightPtr(new Multiplicative());
  else if (type == PARAM_NONE)
    result = DataWeightPtr(new None());

  return result;
}

} /* namespace dataweights */
} /* namespace observations */
} /* namespace niwa */
