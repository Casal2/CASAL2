/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 9/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef REPORTS_MANAGER_H_
#define REPORTS_MANAGER_H_

// Headers
#include <atomic>
#include <thread>
#include <mutex>

#include "../BaseClasses/Manager.h"
#include "../Reports/Report.h"

// Namespaces
namespace niwa {
class Model;
class Runner;
namespace reports {

/**
 * Class Definition
 */
class Manager : public niwa::base::Manager<reports::Manager, niwa::Report> {
  friend class niwa::base::Manager<reports::Manager, niwa::Report>;
  friend class niwa::Managers;
  friend class niwa::Runner;
public:
  // methods
  virtual                     ~Manager() noexcept(true) = default;
  void												Validate() final;
  void                        Validate(shared_ptr<Model> model);
  void												Build() final;
  void                        Build(shared_ptr<Model> model);
  void                        Execute(shared_ptr<Model> model, State::Type model_state);
  void                        Execute(shared_ptr<Model> model, unsigned year, const string& time_step_label);
  void                        Prepare(shared_ptr<Model> model);
  void                        Finalise(shared_ptr<Model> model);
  void                        FlushReports();
  void                        StopThread() { run_.clear(); }
  void                        Pause();
  void                        Resume() { pause_ = false; }
  void                        WaitForReportsToFinish();

  // accessors
  void                        set_report_suffix(const string& suffix);
  const string&               report_suffix() const { return report_suffix_; }

protected:
  // methods
  Manager();

private:
  // Members
  map<State::Type, vector<Report*>> state_reports_;
  map<string, vector<Report*>>      time_step_reports_;
  string                            report_suffix_ = "";
  std::atomic<bool>                 pause_;
  std::atomic<bool>                 is_paused_;
  std::atomic_flag                  run_;
  std::atomic<bool>                 waiting_;
  std::string                       std_header_ = "";
  static std::mutex           			lock_;
  bool															has_validated_ = false;
  bool															has_built_ = false;
  bool															has_prepared_ = false;
  bool															has_finalised_ = false;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_MANAGER_H_ */
