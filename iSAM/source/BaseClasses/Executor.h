/**
 * @file Executor.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/08/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Executors are used as part of a publish/subscribe architecture.
 * Executors can subscribe to timesteps, processes etc.
 *
 */
#ifndef BASE_EXECUTOR_H_
#define BASE_EXECUTOR_H_

// headers
#include "BaseClasses/Object.h"

// namespaces
namespace niwa {
namespace base {

// classes
class Executor : public niwa::base::Object {
public:
  // methods
  Executor() = default;
  virtual ~Executor() = default;

  virtual void                PreExecute()  = 0;
  virtual void                Execute()     = 0;
};

} /* namespace base */
} /* namespace niwa */

#endif /* BASE_EXECUTOR_H_ */
