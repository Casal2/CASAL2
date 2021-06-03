/**
 * @file Thread.cpp
 * @author Scott Rasmussen (scott@zaita.com)
 * @brief A single thread class that is put into the ThreadPool and called to execute
 * model runs.
 * @version 0.1
 * @date 2021-05-10
 *
 * @copyright Copyright (c) 2021
 *
 * Note: There is a bug in MinGW <=10.0 where creating new threads does
 * not reset the floating point math unit on Windows. This is what
 * you will see referred to below when it's mentioned MinGW Bug. We have
 * to manually reset it otherwise we get inconsistent results:
 * https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/fpreset?view=msvc-160
 *
 */

// headers
#include "Thread.h"

#ifdef __MINGW32__
#undef __STRICT_ANSI__
#include <float.h>  // needed for _fpreset(); for MinGW bug
#define __STRICT_ANSI__
#endif  // MINGW32

#include "../Logging/Logging.h"
#include "../Model/Model.h"

// TODO: Move this to the model
#include "../EstimateTransformations/Manager.h"
#include "../Estimates/Manager.h"
#include "../ObjectiveFunction/ObjectiveFunction.h"
#include "../Utilities/To.h"

// namespaces
namespace niwa {

/**
 * Our default constructor
 *
 * @param Shared Pointer to the model this thread is responsible for
 */
Thread::Thread(shared_ptr<Model> model) : model_(model) {}

/**
 * This method will launch our new thread. The thread will call back into this function
 * calling Loop.
 */
void Thread::Launch() {
  std::scoped_lock l(lock_);

  std::function<void()> new_thread([this]() {
#ifdef __MINGW32__
    _fpreset();  // MingW Bug - Need to reset floatpoint prec
#endif
    this->Loop();
  });

  thread_.reset(new std::thread(new_thread));
}

/**
 * This method will join our thread and block until it has been completed
 */
void Thread::Join() {
  LOG_FINEST() << "Joining thread " << thread_->get_id();
  if (thread_->joinable())
    thread_->join();
}

/**
 * WARNING: This function is to only be called by the thread spawned
 * in the this->Launch() method. Do not write any other code to
 * call this function
 */
void Thread::Loop() {
  // Loop while not terminate
  // Note: No lock cause terminate_ is atomic
  while (!terminate_) {
    // Check to see if we have any candidates available for running
    {
      std::scoped_lock l(lock_);
      try {
        if (new_candidates_.size() > 0) {
          LOG_FINEST() << "Thread " << thread_->get_id() << " has model " << model_.get();

          is_finished_ = false;
          candidates_  = new_candidates_;
          new_candidates_.clear();

          //					string cl = "";
          //					for (auto candidate : candidates_)
          //						cl += utilities::ToInline<double, string>(candidate) + ", ";
          //					LOG_MEDIUM() << "[Thread# " << thread_->get_id() << "] Running candidates: " << cl;

          // TODO: Move this to the model
          auto estimates = model_->managers()->estimate()->GetIsEstimated();
          if (candidates_.size() != estimates.size()) {
            LOG_CODE_ERROR() << "The number of enabled estimates does not match the number of test solution values";
          }

          for (unsigned i = 0; i < candidates_.size(); ++i) estimates[i]->set_value(candidates_[i]);

          model_->managers()->estimate_transformation()->RestoreEstimates();
          model_->FullIteration();

          ObjectiveFunction& objective = model_->objective_function();
          objective.CalculateScore();
          //					cout << "Pushing Score: " << objective.score() << " to stack" << endl;
          scores_.push(AS_DOUBLE(objective.score()));

          model_->managers()->estimate_transformation()->TransformEstimates();

          is_finished_ = true;
        }
      } catch (...) {
        cout << "Some shit went wrong" << endl;
      }

      is_finished_ = true;
      // Nothing to do, yield control back to CPU
      std::this_thread::yield();
    }
  }
}

/**
 * Accept some new candidates to be run by our thread.
 * Store them in new candidates so they can be copied into a
 * current candidate vector before we run.
 *
 * @param candidates The new candidates we want to run a model against
 */
void Thread::RunCandidates(const vector<double>& candidates) {
  std::scoped_lock l(lock_);
  new_candidates_ = candidates;
  is_finished_    = false;
}

/**
 * Flag our terminate variable. This is called when we need to wrap up all threads.
 *
 * Note: We don't need a lock because terminate_ is atomic
 */
void Thread::flag_terminate() {
  // std::scoped_lock l(lock_);
  terminate_ = true;
}

/**
 * Check to see if we've finished the current iteration
 *
 * Note: We don't need a lock because is_finished_ is atomic
 */
bool Thread::is_finished() {
  //	std::scoped_lock l(lock_);
  return is_finished_;
}

/**
 * Return the objective score for the model iteration we just ran
 */
double Thread::objective_score() {
  std::scoped_lock l(lock_);
  if (scores_.size() == 0)
    LOG_CODE_ERROR() << "(scores_.size() == 0)";

  double score = scores_.front();
  //	cout << "score is " << score << endl;
  scores_.pop();
  return score;
}

shared_ptr<Model> Thread::model() {
  std::scoped_lock l(lock_);
  return model_;
}

} /* namespace niwa */
