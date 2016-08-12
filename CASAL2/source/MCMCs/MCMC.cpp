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
#include "Reports/Children/MCMCObjective.h"
#include "Reports/Children/MCMCSample.h"

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
  parameters_.Bind<bool>(PARAM_PRINT_DEFAULT_REPORTS, &print_default_reports_, "Indicates if the output prints the default reports", "", true);
}

/**
 * Validate the parameters defined in the configuration file
 */
void MCMC::Validate() {
  parameters_.Populate();
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
  if (!model_->global_configuration().resume() && print_default_reports_) {
    model_->managers().report()->Pause();

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

    model_->managers().report()->Resume();
  } else if (model_->global_configuration().resume() && print_default_reports_) {
    model_->managers().report()->Pause();

    string objective_name = model_->global_configuration().mcmc_objective_file();
    string sample_name    = model_->global_configuration().mcmc_sample_file();
    LOG_FINE() << "Objective file name: " << objective_name;
    LOG_FINE() << "Sample file name: " << sample_name;

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

    model_->managers().report()->Resume();
  }

  DoBuild();
}

/**
 *
 */
void MCMC::Execute() {
  if (model_->global_configuration().create_mpd_file()) {
    configuration::MPD mpd_loader(model_);
    if (!mpd_loader.LoadFile("mpd.out"))
      LOG_FATAL() << "Failed to load MPD Data from mpd.out file";
  }

  DoExecute();
}
} /* namespace niwa */
