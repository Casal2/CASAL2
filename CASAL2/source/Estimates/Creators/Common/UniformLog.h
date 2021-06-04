/**
 * @file UniformLog.h
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
#ifndef ESTIMATES_CREATORS_UNIFORMLOG_H_
#define ESTIMATES_CREATORS_UNIFORMLOG_H_

// headers
#include "../../../Estimates/Creators/Creator.h"

// namespaces
namespace niwa {
namespace estimates {
namespace creators {

/**
 *
 */
class UniformLog : public estimates::Creator {
public:
  UniformLog() = delete;
  explicit UniformLog(shared_ptr<Model> model);
  virtual ~UniformLog() = default;
  void DoCopyParameters(niwa::Estimate* estimate, unsigned index) override final{};
};

} /* namespace creators */
} /* namespace estimates */
} /* namespace niwa */

#endif /* UNIFORMLOG_H_ */
