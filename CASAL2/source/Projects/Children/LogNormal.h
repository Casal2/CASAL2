/**
 * @file LogNormal.h
 * @author  Craig Marsh
 * @date 05/02/2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This projection type will take a mean and cv as input and set the specified parameter
 * as a random draw from a lognormal distribution with the specified mean and cv.
 */
#ifndef PROJECTS_LOGNORMAL_H_
#define PROJECTS_LOGNORMAL_H_

// headers
#include "Projects/Project.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Class definition
 */
class LogNormal : public niwa::Project {
public:
  explicit LogNormal(Model* model);
  virtual                     ~LogNormal() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;
  void                        DoUpdate() override final;

private:
  // members
  Double                      mean_;
  Double                      cv_;
  Double                      value_;
};

} /* namespace projects */
} /* namespace niwa */

#endif /* LOGNORMAL_H_ */
