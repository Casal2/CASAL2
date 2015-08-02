/**
 * @file AgeLength.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 23/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * TODO: Add documentation
 */
#ifndef AGELENGTH_H_
#define AGELENGTH_H_

// headers
#include "BaseClasses/Object.h"

// namespaces
namespace niwa {

// classes
class AgeLength : public niwa::base::Object {
public:
  AgeLength();
  virtual                     ~AgeLength() { };
  void                        Validate();
  void                        Build();
  void                        Reset() { DoReset(); };

  // accessors
  virtual Double              mean_length(unsigned year, unsigned age) = 0;
  virtual Double              mean_weight(unsigned year, unsigned age) = 0;
  virtual void                BuildCv(unsigned year) = 0;
  Double                      cv(unsigned age){return cvs_[age];};

protected:
  // methods
  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoReset() = 0;


  // members
  vector<Double>              time_step_proportions_;
  map<unsigned, Double>       cvs_;
  Double                      cv_first_ = 0.0;
  Double                      cv_last_ = 0.0;
};

// typedef
typedef std::shared_ptr<AgeLength> AgeLengthPtr;

} /* namespace niwa */
#endif /* AGELENGTH_H_ */
