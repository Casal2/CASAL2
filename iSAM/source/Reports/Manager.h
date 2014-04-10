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
#include "BaseClasses/Manager.h"
#include "Reports/Report.h"

// Namespaces
namespace isam {
namespace reports {

/**
 * Class Definition
 */
class Manager : public isam::base::Manager<reports::Manager, isam::Report> {
public:
  Manager();
  virtual                     ~Manager() noexcept(true);
  void                        Build() override final;
  void                        Execute(State::Type model_state);
  void                        Execute(unsigned year, const string& time_step_label);
  void                        FlushCaches();

private:
  // Members
  map<State::Type, vector<ReportPtr> >  state_reports_;
  map<string, vector<ReportPtr> >       time_step_reports_;
};

} /* namespace reports */
} /* namespace isam */
#endif /* REPORTS_MANAGER_H_ */
