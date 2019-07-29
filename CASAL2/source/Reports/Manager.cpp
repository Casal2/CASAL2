/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 15/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#include "Manager.h"

#include "Model/Model.h"

namespace niwa {
namespace reports {

/**
 * Default Constructor
 */
Manager::Manager(Model* model) : model_(model) {
  run_.test_and_set();
  pause_ = false;
  is_paused_ = false;
  waiting_ = false;
}

/**
 * Destructor
 */
Manager::~Manager() noexcept(true) {
}

/**
 * Build our reports then
 * organise the reports stored in our
 * object list into different containers
 * based on their type.
 */
void Manager::Build() {
  if (objects_.size() == 0)
    LOG_WARNING() << "No reports have been specified for this model run";

  LOG_FINEST() << "objects_.size(): " << objects_.size();
  for (auto report : objects_) {
    report->Build();

    if ((RunMode::Type)(report->run_mode() & RunMode::kInvalid) == RunMode::kInvalid)
      LOG_CODE_ERROR() << "Report: " << report->label() << " has not been properly configured to have a run mode";

    if (report->model_state() != State::kExecute) {
      LOG_FINE() << "Adding report " << report->label() << " to state reports, with model->state() = " << report->model_state();
      state_reports_[report->model_state()].push_back(report);
    } else {
      LOG_FINE() << "Adding report " << report->label() << " to time step reports";
      time_step_reports_[report->time_step()].push_back(report);
    }
  }
}

/**
 * Execute any reports that have the model_state
 * specified as their execution state
 *
 * @param model_state The state the model has just finished
 */
void Manager::Execute(State::Type model_state) {
  LOG_TRACE();
  RunMode::Type run_mode = model_->run_mode();
  bool tabular = model_->global_configuration().print_tabular();
  LOG_FINE() << "Checking " << state_reports_[model_state].size() << " reports";
  for(auto report : state_reports_[model_state]) {
      LOG_FINE() << "Checking report: " << report->label();
      if ( (RunMode::Type)(report->run_mode() & run_mode) == run_mode) {
        if (tabular)
          report->ExecuteTabular();
        else
          report->Execute();
      } else
        LOG_FINE() << "Skipping report: " << report->label() << " because run mode is incorrect";
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
  LOG_FINEST() << "year: " << year << "; time_step_label: " << time_step_label << "; reports: " << time_step_reports_[time_step_label].size();

  RunMode::Type run_mode = model_->run_mode();
  bool tabular = model_->global_configuration().print_tabular();
  for(auto report : time_step_reports_[time_step_label]) {
    LOG_FINE() << "executing report " << report->label();
    if ( (RunMode::Type)(report->run_mode() & run_mode) != run_mode) {
      LOG_FINEST() << "Skipping report: " << report->label() << " because run mode is not right";
      continue;
    }
    if (!report->HasYear(year)) {
      LOG_FINEST() << "Skipping report: " << report->label() << " because it does not have year " << year;
      continue;
    }

    if (tabular)
      report->ExecuteTabular();
    else
      report->Execute();
  }
  LOG_TRACE();
}

/**
 *
 */
void Manager::Prepare() {
  LOG_TRACE();
  RunMode::Type run_mode = model_->run_mode();
  bool tabular = model_->global_configuration().print_tabular();
  for (auto report : objects_) {
    if ( (RunMode::Type)(report->run_mode() & run_mode) != run_mode) {
      LOG_FINEST() << "Skipping report: " << report->label() << " because run mode is not right";
      continue;
    }

    if (tabular)
      report->PrepareTabular();
    else
      report->Prepare();
  }
}

/**
 *
 */
void Manager::Finalise() {
  LOG_TRACE();
  RunMode::Type run_mode = model_->run_mode();
  bool tabular = model_->global_configuration().print_tabular();
  for (auto report : objects_) {
    if ( (RunMode::Type)(report->run_mode() & run_mode) != run_mode) {
      LOG_FINEST() << "Skipping report: " << report->label() << " because run mode is not right";
      continue;
    }

    if (tabular)
      report->FinaliseTabular();
    else
      report->Finalise();
  }

  LOG_TRACE();
}

/**
 * This method can be called from the main thread to ensure
 * we wait for all reports to finish
 */
void Manager::WaitForReportsToFinish() {
#ifndef TESTMODE
  waiting_ = true;
  LOG_FINE() << "Waiting for reports";
  while(waiting_);
#endif
  return;
}

/**
 * This method will flush all of the reports to stdout or a file depending on each
 * report when it has finished caching it's output internally.
 *
 * NOTE: This method is called in it's own thread so we can continue to run the model
 * without having to wait for the reports to be ready.
 */
void Manager::FlushReports() {
  // WARNING: DO NOT CALL THIS ANYWHERE. IT'S THREADED
 bool do_break = run_.test_and_set();
 waiting_ = false;
 bool record_waiting = false;
  while(true) {
    while (pause_) {
      is_paused_ = true;
      continue;
    }

    is_paused_ = false;

    if (waiting_)
      record_waiting = true;

    do_break = !run_.test_and_set();
    for (auto report : objects_) {
      if (report->ready_for_writing())
        report->FlushCache();
    }

    if (record_waiting) {
      record_waiting = false;
      waiting_ = false;
    }

    if (do_break)
      break;
  }
}

/**
 *
 */
void Manager::Pause() {
#ifndef TESTMODE
  pause_ = true;
  while(!is_paused_) continue;
#endif
}

} /* namespace reports */
} /* namespace niwa */
