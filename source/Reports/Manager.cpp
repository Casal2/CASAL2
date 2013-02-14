/*
 * Manager.cpp
 *
 *  Created on: 9/01/2013
 *      Author: Admin
 */

#include "Manager.h"

namespace isam {
namespace reports {

Manager::Manager() {
  // TODO Auto-generated constructor stub

}

Manager::~Manager() noexcept(true) {
  // TODO Auto-generated destructor stub
}

/**
 * Build our reports then
 * organise the reports stored in our
 * object list into different containers
 * based on their type.
 */
void Manager::Build() {
  for (ReportPtr report : objects_) {
    report->Build();

    if (report->model_state() != State::kExecute)
      state_reports_[report->model_state()].push_back(report);
    else
      time_step_reports_[report->time_step()].push_back(report);
  }
}

/**
 * Execute any reports that have the model_state
 * specified as their execution state
 *
 * @param model_state The state the model has just finished
 */
void Manager::Execute(State::Type model_state) {
  for(ReportPtr report : state_reports_[model_state]) {
      report->Execute();
  }
}

/**
 * Execute any reports that have the year and
 * time step label as their execution parameters.
 * Note: All these reports are only in the execute phase.
 *
 * @param year The current year for the model
 * @param time_step_label The last time step to be completed
 */
void Manager::Execute(unsigned year, const string& time_step_label) {
  LOG_TRACE();

  for(ReportPtr report : time_step_reports_[time_step_label]) {
    if (report->year() == year)
      report->Execute();
  }
}

} /* namespace reports */
} /* namespace isam */
