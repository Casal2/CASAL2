/**
 * @file AgeLength.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 4/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Age length objects allow the conversion of the partition from an
 * age based structure in to a length based structure and back again
 */
#ifndef AGELENGTH_H_
#define AGELENGTH_H_

#include "BaseClasses/Object.h"

namespace niwa {

class AgeLength : public base::Object {
public:
  AgeLength();
  virtual                     ~AgeLength() = default;
  void                        Validate() { };
  void                        Build() { };
  void                        Reset() { };
};

typedef boost::shared_ptr<AgeLength> AgeLengthPtr;

} /* namespace niwa */

#endif /* AGELENGTH_H_ */
