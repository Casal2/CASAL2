/**
 * @file TimeVarying.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 27/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef TIMEVARYING_H_
#define TIMEVARYING_H_

// headers
#include <boost/shared_ptr.hpp>

#include "BaseClasses/Object.h"
#include "Model/Model.h"


// namespaces
namespace niwa {

/**
 *
 */
class TimeVarying : public niwa::base::Object {
  typedef void (TimeVarying::*UpdateFunction)(Double);
public:
  // methods
  TimeVarying();
  virtual                     ~TimeVarying() = default;
  void                        Validate();
  void                        Build();
  void                        Reset() { };
  void                        Update(unsigned current_year);

protected:
  // methods
  void                        RestoreOriginalValue();
  void                        SetSingleValue(Double value);
  void                        SetVectorValue(Double value);
  void                        SetMapValue(Double value);

  // pure virtual methods
  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoReset() = 0;
  virtual void                DoUpdate() = 0;

  // function pointers
  UpdateFunction              DoUpdateFunc_ = 0;

  // members
  ModelPtr                    model_;
  string                      type_ = "";
  vector<unsigned>            years_;
  string                      parameter_;
  Double                      original_value_ = 0;
  map<unsigned, Double>*      estimable_map_ = 0;
  vector<Double>*             estimable_vector_ = 0;
  Double*                     estimable_ = 0;
};

typedef boost::shared_ptr<TimeVarying> TimeVaryingPtr;

} /* namespace niwa */

#endif /* TIMEVARYING_H_ */
