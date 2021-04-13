/**
 * @file ThreadPool.h
 * @author  Scott Rasmussen (scott@zaita.com)
 * @date 14/10/2019
 * @section LICENSE
 *
 * Copyright NIWA Science ©2019 - www.niwa.co.nz
 *
 * @description
 * This class is our threadpool object. This is the class
 * given to the Minimisers/MCMC etc. When a class gives this
 * class a container of candidates it'll run the model and return
 * the objective function values.
 */
#ifndef SOURCE_THREADPOOL_THREADPOOL_H_
#define SOURCE_THREADPOOL_THREADPOOL_H_

// headers
#include <mutex>
#include <vector>
#include <string>

#include "../ThreadPool/Thread.h"

// namespaces
namespace niwa {

using std::string;
using std::vector;
using std::mutex;

// class declaration
class ThreadPool {
public:
	// methods
	ThreadPool() = default;
	virtual ~ThreadPool() = default;
	void												CreateThreads(vector<shared_ptr<Model>> models);
	void												RunCandidates(const vector<vector<double>>& candidates, vector<double>& scores);
	void												TerminateAll();
	void												CheckThreads();
	void												StressTest();

	vector<shared_ptr<Thread>>	Threads() { return threads_; }

private:
	// methods
	void												Terminate();
	void												JoinAll();

	// members
	vector<shared_ptr<Thread>>	threads_;
	double											scores_[100] = {0};

	DISALLOW_COPY_AND_ASSIGN(ThreadPool);
};

} /* namespace niwa */

#endif /* SOURCE_THREADPOOL_THREADPOOL_H_ */
