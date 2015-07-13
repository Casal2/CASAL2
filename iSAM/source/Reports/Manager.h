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
namespace reports {

/**
 * Class Definition
 */
class Manager : public niwa::oldbase::Manager<reports::Manager, niwa::Report> {
  friend class niwa::oldbase::Manager<reports::Manager, niwa::Report>;
public:
  // methods
  virtual                     ~Manager() noexcept(true);
  void                        Build() override final;
  void                        Execute(State::Type model_state);
  void                        Execute(unsigned year, const string& time_step_label);
  void                        Prepare();
  void                        Finalise();
  void                        FlushReports();
  void                        StopThread() { continue_.clear(); }

  // accessors
  void                        set_report_suffix(const string& suffix) { report_suffix_ = suffix; }
  const string&               report_suffix() const { return report_suffix_; }

protected:
  // methods
  Manager();

private:
  // Members
  map<State::Type, vector<ReportPtr> >  state_reports_;
  map<string, vector<ReportPtr> >       time_step_reports_;
  string                                report_suffix_ = "";
  std::atomic_flag                      continue_;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_MANAGER_H_ */
