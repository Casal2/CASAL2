/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 15/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */
#include "Manager.h"

#include <thread>

#include "../Model/Model.h"

namespace niwa {
namespace reports {

using std::scoped_lock;
std::mutex Manager::lock_;

#define LOCK_WAIT()                                                                                                \
  static std::mutex io_mutex;                                                                                      \
  {                                                                                                                \
    std::lock_guard<std::mutex> lk(io_mutex);                                                                      \
    std::cout << "Model [thread#" << model->id() << "] is waiting for a " << __FUNCTION__ << " lock" << std::endl; \
  }

/**
 * Default Constructor
 */
Manager::Manager() {
  run_.test_and_set();
  pause_     = false;
  is_paused_ = false;
  waiting_   = false;
}

void Manager::AddObject(niwa::Report* object) {
  std::scoped_lock l(lock_);
  objects_.push_back(object);
}

/**
 *
 */
void Manager::Validate() {
  LOG_CODE_ERROR() << "Method not allowed";
}

/**
 *
 */
void Manager::Build() {
  LOG_CODE_ERROR() << "Method not allowed";
}

/**
 *
 */
void Manager::Validate(shared_ptr<Model> model) {
  std::scoped_lock l(lock_);
  if (objects_.size() == 0 || has_validated_) {
    return;
  }

  LOG_FINEST() << "objects_.size(): " << objects_.size();
  for (auto report : objects_) {
    report->Validate(model);
  }

  has_validated_ = true;
}

/**
 * Build our reports then
 * organise the reports stored in our
 * object list into different containers
 * based on their type.
 */
void Manager::Build(shared_ptr<Model> model) {
  std::scoped_lock l(lock_);
  if (objects_.size() == 0 || has_built_ || !has_validated_)
    return;

  LOG_FINEST() << "objects_.size(): " << objects_.size();
  for (auto report : objects_) {
    report->Build(model);

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

  has_built_ = true;
}

/**
 * Execute any reports that have the model_state
 * specified as their execution state
 *
 * @param model_state The state the model has just finished
 */
void Manager::Execute(shared_ptr<Model> model, State::Type model_state) {
  std::scoped_lock l(lock_);

  LOG_TRACE();
  LOG_FINE() << "Executing Models for state: " << (int)model_state;
  if (model_state == State::kFinalise && !model->is_primary_thread_model()) {
    LOG_FINEST() << "Model is not the primary thread model";
    return;
  }

  RunMode::Type run_mode = model->run_mode();
  bool          tabular  = model->global_configuration().print_tabular();
  LOG_FINE() << "Checking " << state_reports_[model_state].size() << " reports";
  for (auto report : state_reports_[model_state]) {
    LOG_FINE() << "Checking report: " << report->label();
    if ((RunMode::Type)(report->run_mode() & run_mode) == run_mode) {
      if (tabular)
        report->ExecuteTabular(model);
      else
        report->Execute(model);
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
void Manager::Execute(shared_ptr<Model> model, unsigned year, const string& time_step_label) {
  std::scoped_lock l(lock_);

  LOG_TRACE();
  LOG_FINEST() << "year: " << year << "; time_step_label: " << time_step_label << "; reports: " << time_step_reports_[time_step_label].size();

  RunMode::Type run_mode = model->run_mode();
  bool          tabular  = model->global_configuration().print_tabular();
  for (auto report : time_step_reports_[time_step_label]) {
    LOG_FINE() << "executing report " << report->label();
    if ((RunMode::Type)(report->run_mode() & run_mode) != run_mode) {
      LOG_FINEST() << "Skipping report: " << report->label() << " because run mode is not right";
      continue;
    }
    if (!report->HasYear(year)) {
      LOG_FINEST() << "Skipping report: " << report->label() << " because it does not have year " << year;
      continue;
    }

    if (tabular)
      report->ExecuteTabular(model);
    else
      report->Execute(model);
  }
  LOG_TRACE();
}

/**
 *
 */
void Manager::Prepare(shared_ptr<Model> model) {
  std::scoped_lock l(lock_);

  LOG_TRACE();
  if (has_prepared_)
    return;

  RunMode::Type run_mode = model->run_mode();
  bool          tabular  = model->global_configuration().print_tabular();
  for (auto report : objects_) {
    if ((RunMode::Type)(report->run_mode() & run_mode) != run_mode) {
      LOG_FINEST() << "Skipping report: " << report->label() << " because run mode is not right";
      continue;
    }

    if (tabular)
      report->PrepareTabular(model);
    else
      report->Prepare(model);
  }

  has_prepared_ = true;
}

/**
 *
 */
void Manager::Finalise(shared_ptr<Model> model) {
  LOG_TRACE();
  std::scoped_lock l(lock_);

  if (has_finalised_) {
    return;
  }

  LOG_FINE() << "finalise called from thread " << std::this_thread::get_id();
  LOG_FINE() << "reports.manager.size(): " << objects_.size();
  for (auto report : objects_) {
    LOG_FINE() << "report: " << report->label() << "." << report->type();
  }

  RunMode::Type run_mode = model->run_mode();
  bool          tabular  = model->global_configuration().print_tabular();
  for (auto report : objects_) {
    if ((RunMode::Type)(report->run_mode() & run_mode) != run_mode) {
      LOG_FINE() << "Skipping report: " << report->label() << " because run mode is not right (" << (int)report->run_mode() << " & " << (int)run_mode << ")";
      continue;
    }

    if (tabular)
      report->FinaliseTabular(model);
    else
      report->Finalise(model);
  }

  LOG_TRACE();
  has_finalised_ = true;
}

/**
 * This method can be called from the main thread to ensure
 * we wait for all reports to finish
 */
void Manager::WaitForReportsToFinish() {
#ifndef TESTMODE
  std::scoped_lock l(lock_);
  waiting_ = true;
  LOG_FINE() << "Waiting for reports";
  while (waiting_)
    ;
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
  bool do_break       = run_.test_and_set();
  waiting_            = false;
  bool record_waiting = false;
  while (true) {
    while (pause_) {
      is_paused_ = true;
      continue;
    }

    is_paused_ = false;

    if (waiting_)
      record_waiting = true;

    do_break = !run_.test_and_set();
    {
      std::scoped_lock l(lock_);
      for (auto report : objects_) {
        if (report->ready_for_writing())
          report->FlushCache();
      }
    }

    if (record_waiting) {
      record_waiting = false;
      waiting_       = false;
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
  while (!is_paused_) {
    continue;
  }
#endif
}

/**
 *
 */
void Manager::set_report_suffix(const string& suffix) {
  std::scoped_lock l(lock_);
  report_suffix_ = suffix;
  for (auto report : objects_) report->set_suffix(suffix);
}

/**
 * @brief Create any reports that we want to be the default.
 * This will create reports the user should be specifying, but
 * maybe has forgot too.
 *
 */
void Manager::CreateDefaultReports() {
  /**
   * We will create the default reports if necessary.
   * But we'll use append if we're resuming.
   *
   * NOTE: Since we're adding reports to the report manager
   * and it's running in a different thread we need to pause
   * and resume the manager thread or we'll get weird crashes.
   */
  // If this is a new mscmc chain (not resuming)
  // if (model_->run_mode() == RunMode::kMCMC) {
  //   if (!model_->global_configuration().resume_mcmc()) {
  //     LOG_MEDIUM() << "Configure resume default MCMC reports";
  //     model_->managers()->report()->Pause();

  //     LOG_MEDIUM() << "Create default MCMC objective function report";
  //     reports::MCMCObjective* objective_report = new reports::MCMCObjective();
  //     objective_report->set_block_type(PARAM_REPORT);
  //     objective_report->set_defined_file_name(__FILE__);
  //     objective_report->set_defined_line_number(__LINE__);
  //     objective_report->parameters().Add(PARAM_LABEL, "mcmc_objective", __FILE__, __LINE__);
  //     objective_report->parameters().Add(PARAM_TYPE, PARAM_MCMC_OBJECTIVE, __FILE__, __LINE__);
  //     objective_report->parameters().Add(PARAM_FILE_NAME, "mcmc_objectives.out", __FILE__, __LINE__);
  //     objective_report->parameters().Add(PARAM_WRITE_MODE, PARAM_INCREMENTAL_SUFFIX, __FILE__, __LINE__);
  //     objective_report->Validate();
  //     model_->managers()->report()->AddObject(objective_report);

  //     LOG_MEDIUM() << "Create default MCMC sample report";
  //     reports::MCMCSample* sample_report = new reports::MCMCSample();
  //     sample_report->set_block_type(PARAM_REPORT);
  //     sample_report->set_defined_file_name(__FILE__);
  //     sample_report->set_defined_line_number(__LINE__);
  //     sample_report->parameters().Add(PARAM_LABEL, "mcmc_sample", __FILE__, __LINE__);
  //     sample_report->parameters().Add(PARAM_TYPE, PARAM_MCMC_SAMPLE, __FILE__, __LINE__);
  //     sample_report->parameters().Add(PARAM_FILE_NAME, "mcmc_samples.out", __FILE__, __LINE__);
  //     sample_report->parameters().Add(PARAM_WRITE_MODE, PARAM_INCREMENTAL_SUFFIX, __FILE__, __LINE__);
  //     sample_report->Validate();
  //     model_->managers()->report()->AddObject(sample_report);

  //     model_->managers()->report()->Resume();
  //   } else if (model_->global_configuration().resume_mcmc()) {
  //     // This is resuming a MCMC
  //     LOG_MEDIUM() << "Resuming MCMC";
  //     model_->managers()->report()->Pause();

  //     string objective_name = model_->global_configuration().mcmc_objective_file();
  //     string sample_name    = model_->global_configuration().mcmc_sample_file();
  //     LOG_MEDIUM() << "MCMC objective function file name: " << objective_name;
  //     LOG_MEDIUM() << "MCMC sample file name: " << sample_name;

  //     if (!model_->managers()->report()->HasType(PARAM_MCMC_OBJECTIVE)) {
  //       LOG_MEDIUM() << "Create default MCMC objective function report";

  //       reports::MCMCObjective* objective_report = new reports::MCMCObjective();
  //       objective_report->set_block_type(PARAM_REPORT);
  //       objective_report->set_defined_file_name(__FILE__);
  //       objective_report->set_defined_line_number(__LINE__);
  //       objective_report->parameters().Add(PARAM_LABEL, "mcmc_objective", __FILE__, __LINE__);
  //       objective_report->parameters().Add(PARAM_TYPE, PARAM_MCMC_OBJECTIVE, __FILE__, __LINE__);
  //       objective_report->parameters().Add(PARAM_FILE_NAME, objective_name, __FILE__, __LINE__);
  //       objective_report->parameters().Add(PARAM_WRITE_MODE, PARAM_APPEND, __FILE__, __LINE__);
  //       objective_report->Validate();
  //       model_->managers()->report()->AddObject(objective_report);
  //     } else {
  //       // Add append to the current report
  //       auto report_ptr = model_->managers()->report()->get(PARAM_MCMC_OBJECTIVE);
  //       report_ptr->set_write_mode(PARAM_APPEND);
  //     }

  //     if (!model_->managers()->report()->HasType(PARAM_MCMC_SAMPLE)) {
  //       LOG_MEDIUM() << "Create default MCMC sample report";

  //       reports::MCMCSample* sample_report = new reports::MCMCSample();
  //       sample_report->set_block_type(PARAM_REPORT);
  //       sample_report->set_defined_file_name(__FILE__);
  //       sample_report->set_defined_line_number(__LINE__);
  //       sample_report->parameters().Add(PARAM_LABEL, "mcmc_sample", __FILE__, __LINE__);
  //       sample_report->parameters().Add(PARAM_TYPE, PARAM_MCMC_SAMPLE, __FILE__, __LINE__);
  //       sample_report->parameters().Add(PARAM_FILE_NAME, sample_name, __FILE__, __LINE__);
  //       sample_report->parameters().Add(PARAM_WRITE_MODE, PARAM_APPEND, __FILE__, __LINE__);
  //       sample_report->Validate();
  //       model_->managers()->report()->AddObject(sample_report);
  //     } else {
  //       // Add append to the current report
  //       auto report_ptr = model_->managers()->report()->get(PARAM_MCMC_SAMPLE);
  //       report_ptr->set_write_mode(PARAM_APPEND);
  //     }

  //     model_->managers()->report()->Resume();
  //   }
  // }
}

} /* namespace reports */
} /* namespace niwa */
