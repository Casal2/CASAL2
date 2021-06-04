/**
 * @file UniformLog.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This prior will return the log(param) when asked for a score
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef ESTIMATES_UNIFORMLOG_H_
#define ESTIMATES_UNIFORMLOG_H_

// Headers
#include "../../Estimates/Estimate.h"

// Namespaces
namespace niwa {
namespace estimates {

/**
 * Class definition
 */
class UniformLog : public niwa::Estimate {
public:
  // Methods
  UniformLog() = delete;
  explicit UniformLog(shared_ptr<Model> model);
  virtual ~UniformLog() = default;
  void   DoValidate() override final{};
  Double GetScore() override final;
};

} /* namespace estimates */
} /* namespace niwa */
#endif /* ESTIMATES_UNIFORMLOG_H_ */
