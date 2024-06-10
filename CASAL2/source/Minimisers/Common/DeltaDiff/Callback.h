/**
 * @file CGammaDiffCallback.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 17/04/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef USE_AUTODIFF
#ifndef MINIMISERS_DELTADIFF_CALLBACK_H_
#define MINIMISERS_DELTADIFF_CALLBACK_H_

// Headers
#include <vector>

#include "../../../ThreadPool/ThreadPool.h"

// namespaces
namespace niwa {
namespace minimisers {
namespace deltadiff {

using std::vector;

/**
 * Class definition
 */
class CallBack {
public:
  CallBack(shared_ptr<ThreadPool> thread_pool);
  virtual ~CallBack() = default;
  double operator()(const vector<double>& Parameters);
  void   operator()(const vector<vector<double>>& Parameters, vector<double>& scores);

  shared_ptr<ThreadPool> thread_pool() { return thread_pool_; }

private:
  shared_ptr<ThreadPool> thread_pool_;
  unsigned               count_ = 0;
};

} /* namespace deltadiff */
}  // namespace minimisers
} /* namespace niwa */

#endif /* MINIMISERS_DELTADIFF_CALLBACK_H_ */
#endif
