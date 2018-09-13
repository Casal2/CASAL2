/**
 * @file Constant.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef PROJECTS_CONSTANT_H_
#define PROJECTS_CONSTANT_H_

// headers
#include "Projects/Project.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Class definition
 */
class Constant : public niwa::Project {
public:
  explicit Constant(Model* model);
  virtual                     ~Constant() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;
  void                        DoUpdate() override final;

protected:
  // members
  vector<Double>              values_;
  Double                      value_;
  map<unsigned, Double>       year_values_;
};

} /* namespace projects */
} /* namespace niwa */

#endif /* CONSTANT_H_ */
