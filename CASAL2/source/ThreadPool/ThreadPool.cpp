/**
 * @file ThreadPool.cpp
 * @author  Scott Rasmussen (scott@zaita.com)
 * @date 14/10/2019
 * @section LICENSE
 *
 * Copyright NIWA Science �2019 - www.niwa.co.nz
 */

// headers
#include "ThreadPool.h"

#include <chrono>

#include "../Logging/Logging.h"
#include "../Model/Managers.h"
#include "../Model/Model.h"

// namespaces
namespace niwa {

/**
 * Create all of our threads. Each thread gets a shared_ptr<Model> object to
 * work with.
 *
 * @param models A vector of Model pointers, length equal to number_of_threads_
 */
void ThreadPool::CreateThreads(vector<shared_ptr<Model>> models) {
  // Create our Thread class objects
  for (auto model : models) {
    auto thread = shared_ptr<Thread>(new Thread(model));
    threads_.push_back(thread);
  }

  // Launch our threads
  for (auto thread : threads_) {
    thread->Launch();
  }
}

/**
 * Run a collection of candidates. This method will iterate over our threads
 * and give any that are not currently processing a set of candidates. Then it'll
 * return a vector of equal length to the candidates containing the objective scores for each
 *
 * @param candidates A vector of candidates (vector of doubles)
 * @return A vector of objective scores matching the length of the candidates vector
 */
void ThreadPool::RunCandidates(const vector<vector<double>>& candidates, vector<double>& scores) {
  if (!is_ok_)
    LOG_CODE_ERROR() << "!is_ok_";
  vector<int> thread_ids(candidates.size(), -1);  // Track which thread is doing the work for each candidate
  using namespace std::chrono_literals;
  LOG_FINE() << "Running a collection of " << candidates.size() << " candidates";
  //	cout << "candidates.size(): " << candidates.size() << endl;
  //	std::this_thread::sleep_for(5s);
  // loop over our candidates, then find a thread for each one to be run in
  unsigned last_thread = 0;
  for (unsigned i = 0; i < candidates.size(); ++i) {
    bool found_thread = false;
    LOG_FINEST() << "Looking for thread for candidate set " << i + 1;
    while (!found_thread) {
      if (last_thread >= threads_.size())
        last_thread = 0;
      for (int thread_idx = last_thread; thread_idx < (int)threads_.size(); ++thread_idx) {
        auto& thread = threads_[thread_idx];
        // is this thread available?
        if (thread->is_finished()) {
          //					cout << "Found thread " << thread_idx << " for candidate set " << i << endl;
          LOG_FINEST() << "Found thread " << thread_idx << " for candidate set " << i;
          LOG_FINEST() << "candidates[i].size(): " << candidates[i].size();

          //					std::this_thread::sleep_for(5s);
          found_thread = true;
          thread->RunCandidates(candidates[i]);
          thread_ids[i] = thread_idx;
          last_thread   = thread_idx + 1;
          break;
        }
      }
    }
  }

  LOG_FINEST() << "All candidates assigned to threads";
  // We've assigned each candidate to a thread, now we'll wait until they're all finished
  for (unsigned i = 0; i < candidates.size(); ++i) {
    auto& thread = threads_[thread_ids[i]];
    // Wait until the thread is finished
    while (!thread->is_finished()) std::this_thread::yield();

    scores[i] = thread->objective_score();

    LOG_FINEST() << "Thread " << thread_ids[i] << " has returned score " << scores[i];
  }

  return;
}

/**
 * Here, we'll call off to terminate all threads and then join them
 * while they're terminating. This will ensure we clean up everything
 * properly
 */
void ThreadPool::TerminateAll() {
  is_ok_ = false;
  Terminate();
  JoinAll();
}

/**
 * Terminate all threads
 */
void ThreadPool::Terminate() {
  for (auto& thread : threads_) {
    if (thread)
      thread->flag_terminate();
  }
}

/**
 * Join all of our threads one at a time.
 * When this method completes, all threads should
 * have finished/terminated
 */
void ThreadPool::JoinAll() {
  for (auto& thread : threads_) {
    if (thread)
      thread->Join();
  }
}

void ThreadPool::CheckThreads() {
  cout << "Checking Model Addresses: " << endl;
  for (auto& thread : threads_) cout << thread->model().get() << endl;

  cout << "Checking Model Managers: " << endl;
  for (auto& thread : threads_) cout << thread->model()->managers().get() << endl;

  cout << "Checking Reports Manager" << endl;
  for (auto& thread : threads_) cout << thread->model()->managers()->report().get() << endl;

  cout << "Checking Process Manager" << endl;
  for (auto& thread : threads_) cout << thread->model()->managers()->process() << endl;
}

void ThreadPool::StressTest() {}

} /* namespace niwa */
