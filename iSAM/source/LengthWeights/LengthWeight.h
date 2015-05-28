/**
 * @file LengthWeight.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef LENGTHWEIGHT_H_
#define LENGTHWEIGHT_H_

// headers
#include "BaseClasses/Object.h"

// namespaces
namespace niwa {

/**
 * class definition
 */
class LengthWeight : public niwa::base::Object {
public:
  // methods
  LengthWeight();
  virtual                     ~LengthWeight() = default;
  void                        Validate();
  void                        Build() { DoBuild(); };
  void                        Reset() { DoReset(); };

  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoReset() = 0;

  // accessors
  virtual Double              mean_weight(Double size, const string &distribution, Double cv) const = 0;
};

// typedef
typedef boost::shared_ptr<LengthWeight> LengthWeightPtr;

} /* namespace niwa */
#endif /* LENGTHWEIGHT_H_ */
