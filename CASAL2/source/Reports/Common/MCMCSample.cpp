/**
 * @file MCMCSample.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "MCMCSample.h"

#include "../../Estimates/Manager.h"
#include "../../MCMCs/MCMC.h"
#include "../../MCMCs/Manager.h"
#include "../../Model/Managers.h"
#include "../../Utilities/String.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
MCMCSample::MCMCSample() {
  run_mode_    = RunMode::kMCMC;
  model_state_ = State::kIterationComplete;
  skip_tags_   = true;
}

/**
 * Build the MCMCSample object
 */
void MCMCSample::DoBuild(shared_ptr<Model> model) {
  mcmc_ = model->managers()->mcmc()->active_mcmc();
  if (!mcmc_)
    LOG_CODE_ERROR() << "mcmc_ = model_->managers()->mcmc()->active_mcmc();";
}

/**
 * Verify the MCMCSample object
 */
void MCMCSample::DoVerify(shared_ptr<Model> model) {
  if (file_name_ == "") {
    LOG_VERIFY() << "in @report " << label_ << " you haven't specified a " << PARAM_FILE_NAME
                 << " this is not recommended, as Casal2 will mix mcmc samples and objectives in the same output file";
  }
}

/**
 * Prepare the MCMCSample object
 */
void MCMCSample::DoPrepare(shared_ptr<Model> model) {
  if (!model->global_configuration().resume_mcmc()) {
    cache_ << ReportHeader(type_, label_, format_);
    auto estimates = model->managers()->estimate()->GetIsEstimated();
    for (unsigned i = 0; i < estimates.size() - 1; ++i) cache_ << estimates[i]->parameter() << " ";
    cache_ << estimates[estimates.size() - 1]->parameter() << REPORT_EOL;
  }
}

/**
 * Print out the MCMC sample values after each iteration
 */
void MCMCSample::DoExecute(shared_ptr<Model> model) {
  if (!mcmc_)
    LOG_CODE_ERROR() << "!mcmc_";
  if (!model)
    LOG_CODE_ERROR() << "!model";

  auto     chain   = mcmc_->chain();
  unsigned element = chain.size() - 1;
  cache_ << utilities::String::join<double>(chain[element].values_, " ") << REPORT_EOL;

  ready_for_writing_ = true;
}

/**
 * Finalise the MCMCSample report
 */
void MCMCSample::DoFinalise(shared_ptr<Model> model) {
  ready_for_writing_ = true;
}

void MCMCSample::DoPrepareTabular(shared_ptr<Model> model) {
  LOG_INFO() << "Tabular mode for reports of type " << PARAM_MCMC_SAMPLE << " has not been implemented";
}

} /* namespace reports */
} /* namespace niwa */
