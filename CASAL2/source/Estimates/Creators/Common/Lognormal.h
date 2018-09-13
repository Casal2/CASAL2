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
namespace niwa {
namespace estimates {
namespace creators {

/**
 *
 */
class Lognormal : public estimates::Creator {
public:
  // methods
  Lognormal() = delete;
  explicit Lognormal(Model* model);
  virtual                     ~Lognormal() = default;
  void                        DoCopyParameters(niwa::Estimate* estimate, unsigned index) override final;

private:
  // members
  vector<Double>              mu_;
  vector<Double>              cv_;
};

} /* namespace creators */
} /* namespace estimates */
} /* namespace niwa */

#endif /* LOGNORMAL_H_ */
