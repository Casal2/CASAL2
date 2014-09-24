/**
 * @file Lognormal.h
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
#ifndef ESTIMATES_CREATORS_LOGNORMAL_H_
#define ESTIMATES_CREATORS_LOGNORMAL_H_

// headers
#include "Estimates/Creators/Creator.h"

// namespaces
namespace isam {
namespace estimates {
namespace creators {

/**
 *
 */
class Lognormal : public estimates::Creator {
public:
  // methods
  Lognormal();
  virtual                     ~Lognormal() = default;
  void                        DoCopyParameters(isam::EstimatePtr estimate, unsigned index) override final;

private:
  // members
  vector<Double>              mu_;
  vector<Double>              cv_;
};

} /* namespace creators */
} /* namespace estimates */
} /* namespace isam */

#endif /* LOGNORMAL_H_ */
