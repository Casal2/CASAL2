/**
 * @file EstimateValue.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 15/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "EstimateValue.h"

#include "Estimates/Manager.h"
#include "Model/Model.h"
#include "Profiles/Manager.h"
#include "Minimisers/Manager.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
EstimateValue::EstimateValue(Model* model) : Report(model) {
  run_mode_     = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kProfiling | RunMode::kProjection);
  model_state_  = State::kIterationComplete;
}

/**
 * Destructor
 */
EstimateValue::~EstimateValue() noexcept(true) {
}

/**
 * Execute this report
 */
void EstimateValue::DoExecute() {
  vector<Estimate*> estimates = model_->managers().estimate()->objects();

  LOG_TRACE();

  // Check if estiamtes are close to bounds. flag a warning.
  for (Estimate* estimate : estimates) {
    if ((estimate->value() - estimate->lower_bound()) < 0.001) {
      LOG_WARNING() << "estimated parameter '" << estimate->parameter() << "' was within 0.001 of lower bound " << estimate->lower_bound();
    } else if ((estimate->upper_bound() - estimate->value()) < 0.001) {
      LOG_WARNING() << "estimated parameter '" << estimate->parameter() << "' was within 0.001 of upper bound " << estimate->upper_bound();
    }
  }

  if (estimates.size() > 0) {
    cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
    cache_ << "values " << REPORT_R_DATAFRAME << "\n";
    for (Estimate* estimate : estimates)
      cache_ << estimate->parameter() << " ";
    cache_ << "\n";
    for (Estimate* estimate : estimates)
      cache_ << AS_VALUE(estimate->value()) << " ";
    cache_ << "\n";

    auto minimiser_ = model_->managers().minimiser()->active_minimiser();
    if (minimiser_) {
      vector<double> est_std_dev(estimates.size(), 0.0);
      covariance_matrix_ = minimiser_->covariance_matrix();
      if (estimates.size() != covariance_matrix_.size1())
        LOG_WARNING() << "number of estimated parameters " << estimates.size() << " does not match the dimension of the covariance matrix "
          << covariance_matrix_.size1();
      for (unsigned i = 0; i < covariance_matrix_.size1(); ++i)
        est_std_dev[i] = sqrt(covariance_matrix_(i, i));

      cache_ << "std_dev " << REPORT_R_DATAFRAME << "\n";
      for (Estimate* estimate : estimates)
        cache_ << estimate->parameter() << " ";
      cache_ << "\n";
      for (auto sd: est_std_dev)
        cache_ << sd << " ";
      cache_ << "\n";
    }

    ready_for_writing_ = true;
  }
}

/**
 *  Execute the report in tabular format
 */
void EstimateValue::DoExecuteTabular() {
  vector<Estimate*> estimates = model_->managers().estimate()->objects();

  /**
   * if this is the first run we print the report header etc
   */
  if (first_run_) {
    first_run_ = false;
    cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
    cache_ << "values " << REPORT_R_DATAFRAME << "\n";
    for (Estimate* estimate : estimates)
      cache_ << estimate->parameter() << " ";
    cache_ << "\n";
  }

  for (Estimate* estimate : estimates)
    cache_ << AS_VALUE(estimate->value()) << " ";
  cache_ <<"\n" ;

  if (estimates.size() > 0) {
    auto minimiser_ = model_->managers().minimiser()->active_minimiser();
    if (minimiser_) {
      covariance_matrix_ = minimiser_->covariance_matrix();
      if (estimates.size() != covariance_matrix_.size1())
        LOG_WARNING() << "The number of estimated parameters " << estimates.size() << " does not match the dimension of the covariance matrix "
          << covariance_matrix_.size1();
      vector<double> est_std_dev(covariance_matrix_.size1(), 0.0);
      for (unsigned i = 0; i < covariance_matrix_.size1(); ++i) {
        est_std_dev[i] = sqrt(covariance_matrix_(i, i));
      }

      for (auto sd: est_std_dev)
        cache_ << sd << " ";
      cache_ << "\n";
    }
  }
}

/**
 *  Finalise the tabular report
 */
void EstimateValue::DoFinaliseTabular() {
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
