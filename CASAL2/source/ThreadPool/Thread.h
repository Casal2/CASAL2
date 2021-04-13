/**
 * @file Thread.h
 * @author  Scott Rasmussen (scott@zaita.com)
 * @date 14/10/2019
 * @section LICENSE
 *
 * Copyright NIWA Science ©2019 - www.niwa.co.nz
 */
#ifndef SOURCE_THREADPOOL_THREAD_H_
#define SOURCE_THREADPOOL_THREAD_H_

// headers
#include <thread>
#include <mutex>
#include <vector>
#include <memory>
#include <atomic>
#include <queue>

#include "../Utilities/NoCopy.h"

// namespaces
namespace niwa {
class Model;
using std::vector;
using std::mutex;
using std::thread;
using std::shared_ptr;

// class declaration
class Thread {
public:
	Thread() = delete;
	explicit Thread(shared_ptr<Model> model);
	virtual ~Thread() = default;
	void												Launch();
	void												Join();
	void												RunCandidates(const vector<double>& candidates);
	void												Loop();

	// accessors
	void												flag_terminate();
	bool												is_finished();
	double											objective_score();
	shared_ptr<Model>						model();

private:
	// members
	shared_ptr<std::thread>			thread_;
	shared_ptr<Model>						model_;
	std::atomic<bool>						is_finished_ = false;
	std::atomic<bool>						terminate_ = false;
	vector<double>							new_candidates_;
	vector<double>							candidates_;
	std::queue<double>					scores_;
	std::mutex									lock_;

	DISALLOW_COPY_AND_ASSIGN(Thread);
};

} /* namespace niwa */

#endif /* SOURCE_THREADPOOL_THREAD_H_ */
