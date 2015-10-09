/**
 * @file NormalByStdev.h
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
#ifndef ESTIMATES_CREATORS_NORMALBYSTDEV_H_
#define ESTIMATES_CREATORS_NORMALBYSTDEV_H_

// headers
#include "Estimates/Creators/Creator.h"

// namespaces
namespace niwa {
namespace estimates {
namespace creators {

/**
 *
 */
class NormalByStdev : public estimates::Creator {
public:
  // methods
  NormalByStdev() = delete;
  explicit NormalByStdev(Model* model);
  virtual                     ~NormalByStdev() = default;
  void                        DoCopyParameters(niwa::Estimate* estimate, unsigned index) override final;

private:
  // members
  vector<Double>              mu_;
  vector<Double>              sigma_;
};

} /* namespace creators */
} /* namespace estimates */
} /* namespace niwa */

#endif /* NORMALBYSTDEV_H_ */
