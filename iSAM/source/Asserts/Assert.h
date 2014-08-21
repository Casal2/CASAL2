/**
 * @file Assert.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 19/08/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * An assert is a runtime check done in the model. If the check
 * fails then the model will stop and the system will return a non-zero value.
 *
 * This is primarily designed for use within unit tests or automated testing of
 * models to ensure the model outputs do not change over time.
 */
#ifndef ASSERT_H_
#define ASSERT_H_

// headers
#include "BaseClasses/Object.h"

// namespaces
namespace isam {

/**
 * Class definition
 */
class Assert : public isam::base::Object {
public:
  // methods
  Assert();
  virtual                     ~Assert() = default;
  void                        Check() { };
  void                        Validate() { };
  void                        Build() { };
  void                        Reset() { };
};

typedef boost::shared_ptr<Assert> AssertPtr;

} /* namespace isam */

#endif /* ASSERT_H_ */
