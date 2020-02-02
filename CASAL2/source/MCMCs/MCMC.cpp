/**
 * @file MCMC.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 10/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "MCMC.h"

#include "ConfigurationLoader/MPD.h"
#include "GlobalConfiguration/GlobalConfiguration.h"
#include "Model/Managers.h"
#include "Model/Model.h"
#include "Reports/Manager.h"
#include "Reports/Common/MCMCObjective.h"
#include "Reports/Common/MCMCSample.h"


// namespaces
namespace niwa {

/**
 * Constructor
 */
MCMC::MCMC(Model* model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label of the MCMC", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of MCMC", "", "");
  parameters_.Bind<unsigned>(PARAM_LENGTH, &length_, "The number of iterations in for the MCMC chain", "");
  parameters_.Bind<bool>(PARAM_ACTIVE, &active_, "Indicates if this is the active MCMC algorithm", "", true);
  // I think we get rid of this option, and don't allow an option for mcmc reports just use this class to create the reports
  //parameters_.Bind<bool>(PARAM_PRINT_DEFAULT_REPORTS, &print_default_reports_, "Indicates if the output prints the default reports", "", true);
  parameters_.Bind<double>(PARAM_STEP_SIZE, &step_size_, "Initial stepsize (as a multiplier of the approximate covariance matrix)", "", 0.02);

}

/**
 * Validate the parameters defined in the configuration file
 */
void MCMC::Validate() {
  parameters_.Populate(model_);
  DoValidate();
}

/**
 * Build any relationships we need and objects that will hold data
 * so they can be used during the execution
 */

void MCMC::Build() {
  LOG_TRACE();
  /**
   * We will create the default reports if necessary.
   * But we'll use append if we're resuming.
   *
   * NOTE: Since we're adding reports to the report manager
   * and it's running in a different thread we need to pause
   * and resume the manager thread or we'll get weird crashes.
   */
  // If this is a new mcmc chain (not resuming)
  if (!model_->global_configuration().resume()) {
    LOG_MEDIUM() << "config resume default reports";
    model_->managers().report()->Pause();

    if (!model_->managers().report()->HasType(PARAM_MCMC_OBJECTIVE)) {
      LOG_MEDIUM() << "create default objective report";
      reports::MCMCObjective* objective_report = new reports::MCMCObjective(model_);
      objective_report->set_block_type(PARAM_REPORT);
      objective_report->set_defined_file_name(__FILE__);
      objective_report->set_defined_line_number(__LINE__);
      objective_report->parameters().Add(PARAM_LABEL, "mcmc_objective", __FILE__, __LINE__);
      objective_report->parameters().Add(PARAM_TYPE, PARAM_MCMC_OBJECTIVE, __FILE__, __LINE__);
      objective_report->parameters().Add(PARAM_FILE_NAME, "mcmc_objectives.out", __FILE__, __LINE__);
      objective_report->parameters().Add(PARAM_WRITE_MODE, PARAM_INCREMENTAL_SUFFIX, __FILE__, __LINE__);
      objective_report->Validate();
      model_->managers().report()->AddObject(objective_report);
    }

    if (!model_->managers().report()->HasType(PARAM_MCMC_SAMPLE)) {
      LOG_MEDIUM() << "create default sample report";

      reports::MCMCSample* sample_report = new reports::MCMCSample(model_);
      sample_report->set_block_type(PARAM_REPORT);
      sample_report->set_defined_file_name(__FILE__);
      sample_report->set_defined_line_number(__LINE__);
      sample_report->parameters().Add(PARAM_LABEL, "mcmc_sample", __FILE__, __LINE__);
      sample_report->parameters().Add(PARAM_TYPE, PARAM_MCMC_SAMPLE, __FILE__, __LINE__);
      sample_report->parameters().Add(PARAM_FILE_NAME, "mcmc_samples.out", __FILE__, __LINE__);
      sample_report->parameters().Add(PARAM_WRITE_MODE, PARAM_INCREMENTAL_SUFFIX, __FILE__, __LINE__);
      sample_report->Validate();
      model_->managers().report()->AddObject(sample_report);
    }

    model_->managers().report()->Resume();
  } else if (model_->global_configuration().resume()) {
    // This is resuming a MCMC
    LOG_MEDIUM() << "Resuming mcmc";
    model_->managers().report()->Pause();

    string objective_name = model_->global_configuration().mcmc_objective_file();
    string sample_name    = model_->global_configuration().mcmc_sample_file();
    LOG_MEDIUM() << "Objective file name: " << objective_name;
    LOG_MEDIUM() << "Sample file name: " << sample_name;

    if (!model_->managers().report()->HasType(PARAM_MCMC_OBJECTIVE)) {
      // create the default report
      reports::MCMCObjective* objective_report = new reports::MCMCObjective(model_);
      objective_report->set_block_type(PARAM_REPORT);
      objective_report->set_defined_file_name(__FILE__);
      objective_report->set_defined_line_number(__LINE__);
      objective_report->parameters().Add(PARAM_LABEL, "mcmc_objective", __FILE__, __LINE__);
      objective_report->parameters().Add(PARAM_TYPE, PARAM_MCMC_OBJECTIVE, __FILE__, __LINE__);
      objective_report->parameters().Add(PARAM_FILE_NAME, objective_name, __FILE__, __LINE__);
      objective_report->parameters().Add(PARAM_WRITE_MODE, PARAM_APPEND, __FILE__, __LINE__);
      objective_report->Validate();
      model_->managers().report()->AddObject(objective_report);
    } else {
      // Add append to the current report
      auto report_ptr = model_->managers().report()->GetReport(PARAM_MCMC_OBJECTIVE);
      report_ptr->set_write_mode(PARAM_APPEND);
    }

    if (!model_->managers().report()->HasType(PARAM_MCMC_SAMPLE)) {
      reports::MCMCSample* sample_report = new reports::MCMCSample(model_);
      sample_report->set_block_type(PARAM_REPORT);
      sample_report->set_defined_file_name(__FILE__);
      sample_report->set_defined_line_number(__LINE__);
      sample_report->parameters().Add(PARAM_LABEL, "mcmc_sample", __FILE__, __LINE__);
      sample_report->parameters().Add(PARAM_TYPE, PARAM_MCMC_SAMPLE, __FILE__, __LINE__);
      sample_report->parameters().Add(PARAM_FILE_NAME, sample_name, __FILE__, __LINE__);
      sample_report->parameters().Add(PARAM_WRITE_MODE, PARAM_APPEND, __FILE__, __LINE__);
      sample_report->Validate();
      model_->managers().report()->AddObject(sample_report);
    } else {
      // Add append to the current report
      auto report_ptr = model_->managers().report()->GetReport(PARAM_MCMC_SAMPLE);
      report_ptr->set_write_mode(PARAM_APPEND);
    }
    model_->managers().report()->Resume();
  }

  DoBuild();
}

/**
 *
 */
void MCMC::Execute() {
	LOG_FINE() << "executing MCMC";
  if (model_->global_configuration().create_mpd_file()) {
    configuration::MPD mpd_loader(model_);
    if (!mpd_loader.LoadFile("mpd.out"))
      LOG_FATAL() << "Failed to load MPD Data from mpd.out file";
  }

  DoExecute();
}
} /* namespace niwa */
