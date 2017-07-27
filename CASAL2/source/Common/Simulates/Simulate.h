/**
 * @file Simulate.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 5/08/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SIMULATE_H_
#define SIMULATE_H_

// headers
#include "Common/BaseClasses/Object.h"
#include "Common/Model/Model.h"

// namespaces
namespace niwa {

/**
 * Class definition
 */
class Simulate : public niwa::base::Object {
public:
  typedef void (Simulate::*UpdateFunction)(Double);

  // methods
  Simulate() = delete;
  Simulate(Model* model);
  virtual                     ~Simulate() = default;
  void                        Validate();
  void                        Build();
  void                        Reset() { DoReset(); };
  void                        Update(unsigned current_year);

protected:
  // methods
  void                        RestoreOriginalValue();
  void                        set_single_value(Double value);
  void                        set_vector_value(Double value);
  void                        set_map_value(Double value);

  // pure virtual methods
  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoReset() = 0;
  virtual void                DoUpdate() = 0;

  // function pointers
  UpdateFunction              update_function_ = 0;

  // members
  Model*                      model_;
  string                      type_ = "";
  vector<unsigned>            years_;
  string                      parameter_;
  Double                      original_value_ = 0;
  map<unsigned, Double>*      addressable_map_ = 0;
  vector<Double>*             addressable_vector_ = 0;
  Double*                     addressable_ = 0;
};
} /* namespace niwa */

#endif /* PROJECT_H_ */
