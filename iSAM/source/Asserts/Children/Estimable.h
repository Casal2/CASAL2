/**
 * @file Estimable.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 1/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * TODO: Add documentation
 */
#ifndef ASSERT_ESTIMABLE_H_
#define ASSERT_ESTIMABLE_H_

// headers
#include "Asserts/Assert.h"

// namespaces
namespace niwa {
namespace asserts {

// classes
class Estimable : public niwa::Assert {
public:
  // methods
  Estimable(Model* model);
  virtual                     ~Estimable() = default;
  void                        Execute() override final;

protected:
  // methods
  void                        DoValidate() override final;
  void                        DoBuild() override final;

private:
  // members
  string                      parameter_ = "";
  vector<unsigned>            years_;
  string                      time_step_label_ = "";
  vector<unsigned>            values_;
  map<unsigned, double>       year_values_;
  Double*                     estimable_ = nullptr;
};

} /* namespace asserts */
} /* namespace niwa */

#endif /* ESTIMABLE_H_ */
