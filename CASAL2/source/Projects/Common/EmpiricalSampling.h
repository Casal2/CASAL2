/**
 * @file EmpiricalSampling.h
 * @author  Craig Marsh
 * @date 05/02/2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This projection class will take a parameter and a year range that this parameter exists for. This projection class will randomly resample with replacement
 * from the old parameter between the year range, for each year specified in the @project block
 */
#ifndef PROJECTS_EMPIRICAL_SAMPLING_H_
#define PROJECTS_EMPIRICAL_SAMPLING_H_

// headers
#include "Projects/Project.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Class definition
 */
class EmpiricalSampling : public niwa::Project {
public:
  explicit EmpiricalSampling(Model* model);
  virtual                     ~EmpiricalSampling() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;
  void                        DoUpdate() override final;

private:
  // members
  unsigned                    start_year_;
  unsigned                    final_year_;
  Double                      value_;
  map<unsigned,unsigned>      resampled_years_;
};

} /* namespace projects */
} /* namespace niwa */

#endif /* PROJECTS_EMPIRICAL_SAMPLING_H_ */
