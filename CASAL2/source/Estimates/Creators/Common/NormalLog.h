/**
 * @file NormalLog.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/09/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/�2014 - www.niwa.co.nz
 *
 */
#ifndef ESTIMATES_CREATORS_NORMALLOG_H_
#define ESTIMATES_CREATORS_NORMALLOG_H_

// headers
#include "../../../Estimates/Creators/Creator.h"

// namespaces
namespace niwa {
namespace estimates {
namespace creators {

/**
 *
 */
class NormalLog : public estimates::Creator {
public:
  NormalLog() = delete;
  explicit NormalLog(shared_ptr<Model> model);
  virtual ~NormalLog() = default;
  void DoCopyParameters(niwa::Estimate* estimate, unsigned index) override final;

private:
  // members
  vector<Double> mu_;
  vector<Double> sigma_;
};

} /* namespace creators */
} /* namespace estimates */
} /* namespace niwa */

#endif /* NORMALLOG_H_ */
