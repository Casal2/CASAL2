/**
 * @file NormalLog.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifndef ESTIMATES_CREATORS_NORMALLOG_H_
#define ESTIMATES_CREATORS_NORMALLOG_H_

// headers
#include "Estimates/Creators/Creator.h"

// namespaces
namespace niwa {
namespace estimates {
namespace creators {

/**
 *
 */
class NormalLog : public estimates::Creator {
public:
  NormalLog();
  virtual                     ~NormalLog() = default;
  void                        DoCopyParameters(niwa::EstimatePtr estimate, unsigned index) override final;

private:
  // members
  vector<Double>              mu_;
  vector<Double>              sigma_;
};

} /* namespace creators */
} /* namespace estimates */
} /* namespace niwa */

#endif /* NORMALLOG_H_ */
