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
#include <memory>

#include "BaseClasses/Object.h"

// namespaces
namespace niwa {
namespace base {

// classes
class Executor : public niwa::base::Object, public std::enable_shared_from_this<Executor> {
public:
  // methods
  Executor() = default;
  virtual ~Executor() = default;

  virtual void                PreExecute()  = 0;
  virtual void                Execute()     = 0;

  // accessors
  std::shared_ptr<Executor> shared_ptr() { return shared_from_this(); }
};

} /* namespace base */
} /* namespace niwa */

typedef std::shared_ptr<niwa::base::Executor> ExecutorPtr;

#endif /* BASE_EXECUTOR_H_ */
