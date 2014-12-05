/**
 * @file AgeSize.h
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
#ifndef AGESIZE_H_
#define AGESIZE_H_

// headers
#include "BaseClasses/Object.h"

// namespaces
namespace niwa {

/**
 * class definition
 */
class AgeSize : public niwa::base::Object {
public:
  AgeSize();
  virtual                     ~AgeSize() { };
  void                        Validate();
  void                        Build();
  void                        Reset() { DoReset(); };

  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoReset() = 0;

  // accessors
  virtual Double              mean_size(unsigned year, unsigned age) = 0;
  virtual Double              mean_weight(unsigned year, unsigned age) = 0;

protected:
  // members
  vector<Double>              time_step_proportions_;
};

// typedef
typedef boost::shared_ptr<AgeSize> AgeSizePtr;

} /* namespace niwa */
#endif /* AGESIZE_H_ */
