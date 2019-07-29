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

#include "BaseClasses/Manager.h"
#include "Reports/Report.h"

// Namespaces
namespace niwa {
class Model;

namespace reports {

/**
 * Class Definition
 */
class Manager : public niwa::base::Manager<reports::Manager, niwa::Report> {
  friend class niwa::base::Manager<reports::Manager, niwa::Report>;
  friend class niwa::Managers;
public:
  // methods
  virtual                     ~Manager() noexcept(true);
  void                        Build() override final;
  void                        Execute(State::Type model_state);
  void                        Execute(unsigned year, const string& time_step_label);
  void                        Prepare();
  void                        Finalise();
  void                        FlushReports();
  void                        StopThread() { run_.clear(); }
  void                        Pause();
  void                        Resume() { pause_ = false; }
  void                        WaitForReportsToFinish();

  // accessors
  void                        set_report_suffix(const string& suffix) { report_suffix_ = suffix; }
  const string&               report_suffix() const { return report_suffix_; }
  const string&               std_header() const { return std_header_; }
  void                        set_std_header(const string& header) { std_header_ = header; }

protected:
  // methods
  Manager() = delete;
  explicit Manager(Model* model);

private:
  // Members
  map<State::Type, vector<Report*>> state_reports_;
  map<string, vector<Report*>>      time_step_reports_;
  string                            report_suffix_ = "";
  std::atomic<bool>                 pause_;
  std::atomic<bool>                 is_paused_;
  std::atomic_flag                  run_;
  std::atomic<bool>                 waiting_;
  Model*                            model_;
  std::string                       std_header_ = "";
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_MANAGER_H_ */
