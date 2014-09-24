/**
 * @file Uniform.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef ESTIMATES_CREATORS_UNIFORM_H_
#define ESTIMATES_CREATORS_UNIFORM_H_

// headers
#include "Estimates/Creators/Creator.h"

// namespaces
namespace isam {
namespace estimates {
namespace creators {

/**
 *
 */
class Uniform : public estimates::Creator {
public:
  // methods
  Uniform();
  virtual                     ~Uniform() = default;
  void                        DoCopyParameters(isam::EstimatePtr estimate, unsigned index) override final { };
};

} /* namespace creators */
} /* namespace estimates */
} /* namespace isam */

#endif /* ESTIMATES_CREATORS_UNIFORM_H_ */
