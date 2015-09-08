/**
 * @file Managers.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 11/08/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class holds accessors for the managers in the application. I've gone down
 * this path because I want to be able to easily mock them for use in the unit
 * test suite.
 */
#ifndef SOURCE_MODEL_MANAGERS_H_
#define SOURCE_MODEL_MANAGERS_H_

// headers
#include "Estimables/Estimables.h"
#include "LengthWeights/Manager.h"
#include "Selectivities/Manager.h"
#include "TimeSteps/Manager.h"

// namespaces
namespace niwa {

// classes
class Managers {
  friend class Model;
public:
  // accessors
  virtual Estimables&                   estimables() { return Estimables::Instance(); }
  virtual lengthweights::Manager&       length_weight() { return lengthweights::Manager::Instance(); }
  virtual selectivities::Manager&       selectivity() { return selectivities::Manager::Instance(); }
  virtual timesteps::Manager&           time_step() { return timesteps::Manager::Instance(); }

protected:
  // methods
  Managers() = default;
  virtual                     ~Managers() = default;
};

} /* namespace niwa */

#endif /* SOURCE_MODEL_MANAGERS_H_ */
