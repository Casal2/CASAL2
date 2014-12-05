/**
 * @file SizeWeight.h
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
#ifndef SIZEWEIGHT_H_
#define SIZEWEIGHT_H_

// headers
#include "BaseClasses/Object.h"

// namespaces
namespace niwa {

/**
 * class definition
 */
class SizeWeight : public niwa::base::Object {
public:
  // methods
  SizeWeight();
  virtual                     ~SizeWeight() = default;
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
typedef boost::shared_ptr<SizeWeight> SizeWeightPtr;

} /* namespace niwa */
#endif /* SIZEWEIGHT_H_ */
