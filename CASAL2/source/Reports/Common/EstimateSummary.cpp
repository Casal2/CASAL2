/**
 * @file EstimateSummary.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 27/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "EstimateSummary.h"

#include "../../Estimates/Manager.h"
#include "../../Minimisers/Manager.h"

// Namespaces
namespace niwa {
namespace reports {

/**
 * Default Constructor
 */
EstimateSummary::EstimateSummary() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kProfiling);
  model_state_ = State::kIterationComplete;
}

/**
 * Execute the estimate summary report
 */
void EstimateSummary::DoExecute(shared_ptr<Model> model) {
  // Print the estimates
  vector<Estimate*> estimates = model->managers()->estimate()->GetIsEstimated();

  auto           minimiser_ = model->managers()->minimiser()->active_minimiser();
  vector<Double> est_std_dev(estimates.size(), 0.0);
  if (minimiser_) {
    covariance_matrix_ = minimiser_->covariance_matrix();
    if (model->run_mode() == RunMode::kEstimation && estimates.size() != covariance_matrix_.size1())
      LOG_WARNING() << "The number of estimated parameters " << estimates.size() << " does not match the dimension of the covariance matrix " << covariance_matrix_.size1();
    if (covariance_matrix_.size1() > 0)
      for (unsigned i = 0; i < covariance_matrix_.size1(); ++i) est_std_dev[i] = sqrt(covariance_matrix_(i, i));
  }
  cache_ << ReportHeader(type_, label_, format_);
  unsigned est_idx = 0;
  for (Estimate* estimate : estimates) {
    cache_ << estimate->parameter() << " " << REPORT_R_LIST << REPORT_EOL;
    cache_ << "value: " << AS_DOUBLE(estimate->value()) << REPORT_EOL;
    cache_ << "initial_value: " << AS_DOUBLE(estimate->get_initial_value()) << REPORT_EOL;
    cache_ << "phase: " << estimate->phase() << REPORT_EOL;
    vector<Double> prior_values =  estimate->GetPriorValues();
    vector<string> prior_labs =  estimate->GetPriorLabels();
    cache_ << "hyperparameters:";
    for(unsigned i = 0; i < prior_labs.size(); ++i) 
      cache_ << " " << prior_labs[i];
    cache_ << REPORT_EOL;
    cache_ << "hyperparameter_values:";
    for(unsigned i = 0; i < prior_labs.size(); ++i) 
      cache_ << " " << prior_values[i];
    cache_ << REPORT_EOL;
    // NOTE: this assumes that the estimated parameters and the covariance matrix are in the same order
    if (model->run_mode() == RunMode::kEstimation && minimiser_)
      cache_ << "std_dev: " << est_std_dev[est_idx] << REPORT_EOL;
    est_idx++;

    // also output label, lower_bound, upper_bound, etc.
    map<string, Parameter*> parameters = estimate->parameters().parameters();
    for (auto iter = parameters.begin(); iter != parameters.end(); ++iter) {
      cache_ << iter->first << ": ";
      for (string parameter_value : iter->second->values()) cache_ << parameter_value << " ";
      cache_ << REPORT_EOL;
    }
    cache_ << REPORT_R_LIST_END << REPORT_EOL << REPORT_EOL;
  }

  ready_for_writing_ = true;
}
void EstimateSummary::DoPrepareTabular(shared_ptr<Model> model) {
  LOG_INFO() << "Tabular mode for reports of type " << PARAM_ESTIMATE_SUMMARY << " has not been implemented";
}

} /* namespace reports */
} /* namespace niwa */
