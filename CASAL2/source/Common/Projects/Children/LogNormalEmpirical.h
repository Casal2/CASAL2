/**
 * @file LogNormalEmpirical.h
 * @author  Craig Marsh
 * @date 07/04/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This projection type will take a mean, sigma, rho and multiplier as input and set the specified parameter
 * as a random draw from a gaussian AR(1) process. I have actually removed the AR(1) process as I don't think it is correct.
 */
#ifndef LOGNORMAL_EMPIRICAL_H_
#define LOGNORMAL_EMPIRICAL_H_

// headers
#include "Common/Projects/Project.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Class definition
 */
class LogNormalEmpirical : public niwa::Project {
public:
  explicit LogNormalEmpirical(Model* model);
  virtual                     ~LogNormalEmpirical() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final { };
  void                        DoReset() override final;
  void                        DoUpdate() override final;

private:
  // members
  unsigned                    start_year_;
  unsigned                    final_year_;
  map<unsigned,unsigned>      resampled_years_;
  Double                      mean_ = 0.0;
  Double                      sigma_ = 0.0;
  Double                      value_;
  Double                      last_value_;
  map<unsigned,Double>        normal_draw_by_year_;
};

} /* namespace projects */
} /* namespace niwa */

#endif /* LOGNORMAL_EMPIRICAL_H_ */
