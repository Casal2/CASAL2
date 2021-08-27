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
#include "Factory.h"

namespace niwa::reports {

using std::scoped_lock;
std::mutex Manager::lock_;
std::mutex Manager::internal_lock_;

/**
 * @brief Construct a new Manager:: Manager object
 *
 */
Manager::Manager() {
  run_.test_and_set();
  pause_     = false;
  is_paused_ = false;
  waiting_   = false;
}

/**
 * @brief
 *
 * @param object
 */
void Manager::AddObject(niwa::Report* object) {
  std::scoped_lock l(lock_);
  objects_.push_back(object);
}

/**
 * @brief Add a new internal report. The new report should be at the same state level
 * as the object creating it. So if you're creating it from a validate method,
 * then it should be given in a way that is ready to be (but has not been) validated.
 *
 * If you're adding an internal object during a build phase, then you will need to ensure
 * it has been successfully validated and is ready for this manager to call build on it.
 *
 * @param object Pointer to report object you have created
 */
void Manager::AddInternalObject(niwa::Report* object) {
  std::scoped_lock l(internal_lock_);
  internal_objects_.push_back(object);
}

/**
 * @brief
 *
 */
void Manager::Validate() {
  LOG_CODE_ERROR() << "Method not allowed";
}

/**
 * @brief
 *
 */
void Manager::Build() {
  LOG_CODE_ERROR() << "Method not allowed";
}

/**
 * @brief Validate the internal consistency of each report object
 *
 * @param model A model pointer so we can access global configuration
 */
void Manager::Validate(shared_ptr<Model> model) {
  std::scoped_lock l(lock_);
  if (objects_.size() == 0 || has_validated_)
    return;
  if (model->global_configuration().disable_all_reports())
    return;

  LOG_FINEST() << "objects_.size(): " << objects_.size();
  for (auto report : objects_) {
    if (report->is_valid())
      report->Validate(model);
  }

  // move any objects from the internal vector to the objects vector
  // internal vector is used for auto generated reports
  for (auto report : internal_objects_) {
    report->Validate(model);
    objects_.push_back(report);
  }
  internal_objects_.clear();

  has_validated_ = true;
}

/**
 * @brief Build our reports then
 * organise the reports stored in our
 * object list into different containers
 * based on their type.
 *
 * @param model
 */
void Manager::Build(shared_ptr<Model> model) {
  std::scoped_lock l(lock_);
  if (objects_.size() == 0 || has_built_ || !has_validated_)
    return;
  if (model->global_configuration().disable_all_reports())
    return;

  RunMode::Type run_mode = model->run_mode();

  std::map<std::string, int> count_file_names;
  std::map<std::string, int> count_labels;
  for (auto report : objects_) {
    // Count duplicate file names
    auto file_result = count_file_names.insert(std::pair<std::string, int>(report->file_name(), 1));
    if (file_result.second == false)
      file_result.first->second++;
    // Count duplicate labels
    auto label_result = count_labels.insert(std::pair<std::string, int>(report->label(), 1));
    if (label_result.second == false)
      label_result.first->second++;
  }
  // Count duplicate file names
  for (auto& file_result : count_file_names) {
    if (file_result.first != "" && file_result.second > 1) {
      LOG_ERROR() << "The file name " << file_result.first << " occurs twice in the requested reports. File names can only be used once";
      exit(-1);
    }
  }
  // Count duplicate labels
  for (auto& label_result : count_labels) {
    if (label_result.first != "" && label_result.second > 1) {
      LOG_ERROR() << "The label " << label_result.first << " occurs twice in the requested reports. Labels can only be used once";
      exit(-1);
    }
  }

  LOG_FINEST() << "objects_.size(): " << objects_.size();
  for (auto report : objects_) {
    if ((RunMode::Type)(report->run_mode() & run_mode) == run_mode)
      if (report->is_valid())
        report->Build(model);
  }

  // move any objects from the internal vector to the objects vector
  // internal vector is used for auto generated reports
  for (auto report : internal_objects_) {
    report->Build(model);
    objects_.push_back(report);
  }
  internal_objects_.clear();

  /**
   * Iterate over the objects doing some basic checks.
   * - Has the report been configured to have a proper run mode
   * - Take report and add it to the state or time step report lists based on it's execution type
   * - Check if we have some basic reports for the current run mode
   */
  [[maybe_unused]] bool exists_MCMC_sample    = false;  // Ignore unused because
  [[maybe_unused]] bool exists_MCMC_objective = false;  // we only check if it's valid
  [[maybe_unused]] bool exists_estimate_value = false;  // during non-test modes

  for (auto report : objects_) {
    if ((RunMode::Type)(report->run_mode() & RunMode::kInvalid) == RunMode::kInvalid)
      LOG_CODE_ERROR() << "Report: " << report->label() << " has not been properly configured to have a run mode";

    // add report to the right map for executing later
    if (report->model_state() != State::kExecute) {
      LOG_FINE() << "Adding report " << report->label() << " to state reports, with model->state() = " << report->model_state();
      state_reports_[report->model_state()].push_back(report);
    } else {
      LOG_FINE() << "Adding report " << report->label() << " to time step reports";
      time_step_reports_[report->time_step()].push_back(report);
    }

    // Do we have the reports we expect to have for this run mode
    if (util::ToLowercase(report->type()) == PARAM_MCMC_SAMPLE)
      exists_MCMC_sample = true;
    if (util::ToLowercase(report->type()) == PARAM_MCMC_OBJECTIVE)
      exists_MCMC_objective = true;
    if (util::ToLowercase(report->type()) == PARAM_ESTIMATE_VALUE)
      exists_estimate_value = true;
  }

// Pop out any warnings if we're missing reports for specific run modes
// don't do it during unit tests though as warnings become errors
#ifndef TESTMODE
  if (run_mode == RunMode::Type::kMCMC && !exists_MCMC_sample)
    LOG_WARNING() << "You are running an MCMC but there was no " << PARAM_MCMC_SAMPLE << " report specified. This is probably an error";
  if (run_mode == RunMode::Type::kMCMC && !exists_MCMC_objective)
    LOG_WARNING() << "You are running an MCMC but there was no " << PARAM_MCMC_OBJECTIVE << " report specified. This is probably an error";
  if (run_mode == RunMode::Type::kEstimation && !exists_estimate_value)
    LOG_WARNING() << "You are running an estimation but there was no " << PARAM_ESTIMATE_VALUE << " report specified. This is probably an error";
#endif

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
  if (model->global_configuration().disable_all_reports())
    return;

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
    if ((RunMode::Type)(report->run_mode() & run_mode) == run_mode && report->is_valid()) {
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

  if (model->global_configuration().disable_all_reports())
    return;
  LOG_FINEST() << "year: " << year << "; time_step_label: " << time_step_label << "; reports: " << time_step_reports_[time_step_label].size();

  RunMode::Type run_mode = model->run_mode();
  bool          tabular  = model->global_configuration().print_tabular();
  for (auto report : time_step_reports_[time_step_label]) {
    LOG_FINE() << "executing report " << report->label();
    if ((RunMode::Type)(report->run_mode() & run_mode) != run_mode && report->is_valid()) {
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
  if (model->global_configuration().disable_all_reports())
    return;

  RunMode::Type run_mode = model->run_mode();
  bool          tabular  = model->global_configuration().print_tabular();
  for (auto report : objects_) {
    if ((RunMode::Type)(report->run_mode() & run_mode) != run_mode) {
      LOG_FINEST() << "Skipping report: " << report->label() << " because run mode is not right";
      continue;
    }
    if (!report->is_valid())
      continue;
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

  if (has_finalised_)
    return;
  if (model->global_configuration().disable_all_reports())
    return;

  LOG_FINE() << "finalise called from thread " << std::this_thread::get_id();
  LOG_FINE() << "reports.manager.size(): " << objects_.size();

  RunMode::Type run_mode = model->run_mode();
  bool          tabular  = model->global_configuration().print_tabular();
  for (auto report : objects_) {
    if ((RunMode::Type)(report->run_mode() & run_mode) != run_mode) {
      LOG_FINE() << "Skipping report: " << report->label() << " because run mode is not right (" << (int)report->run_mode() << " & " << (int)run_mode << ")";
      continue;
    }
    if (!report->is_valid())
      continue;

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
  // TODO use: add_internal to avoid thread issues.
  /**
   * We will create the default reports if necessary.
   * But we'll use append if we're resuming.
   *
   * NOTE: Since we're adding reports to the report manager
   * and it's running in a different thread we need to pause
   * and resume the manager thread or we'll get weird crashes.
   */
  // If this is a new mcmc chain (not resuming)
  // if (model_->run_mode() == RunMode::kMCMC) {
  //   if (!model_->global_configuration().resume_mcmc()) {
  //     LOG_MEDIUM() << "Configure resume default MCMC reports";
  //     model_->managers()->report()->Pause();

  //     LOG_MEDIUM() << "Create default MCMC objective function report";
  //     reports::MCMCObjective* objective_report = new reports::MCMCObjective();
  //     objective_report->set_block_type(PARAM_REPORT);
  //     objective_report->set_defined_file_name(__FILE__);
  //     objective_report->set_defined_line_number(__LINE__);
  //     objective_report->parameters().Add(PARAM_LABEL, "__objective", __FILE__, __LINE__);
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
  //     sample_report->parameters().Add(PARAM_LABEL, "__sample", __FILE__, __LINE__);
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

}  // namespace niwa::reports
