/**
 * @file Catchability.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Catchabilities are used to scale up or down observations with expectations
 *
 */
#ifndef CATCHABILITY_H_
#define CATCHABILITY_H_

// Headers
#include "BaseClasses/Object.h"

// Namespaces
namespace niwa {

// classes
class Catchability : public niwa::base::Object {
public:
  // Methods
  Catchability();
  virtual                     ~Catchability() = default;
  void                        Validate();
  void                        Build() { };
  void                        Reset() { };
  virtual void                DoValidate() = 0;

  // Accessors
  virtual Double              q() const = 0;
};

} /* namespace niwa */
#endif /* CATCHABILITY_H_ */
