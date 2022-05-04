/**
 * @file EstimateValue.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 15/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */

// headers
#include "EstimateValue.h"

#include "../../Estimates/Manager.h"
#include "../../Model/Model.h"
#include "../../Profiles/Manager.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
EstimateValue::EstimateValue() {
  LOG_TRACE();

  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kProfiling | RunMode::kProjection);
  model_state_ = State::kIterationComplete;
}

/**
 * Execute this report.
 */
void EstimateValue::DoExecute(shared_ptr<Model> model) {
  vector<Estimate*> estimates = model->managers()->estimate()->GetIsEstimated();
  vector<Profile*>  profiles  = model->managers()->profile()->objects();
  LOG_TRACE();
  // Check if estimates are close to bounds. flag a warning.
  for (Estimate* estimate : estimates) {
    if ((estimate->value() - estimate->lower_bound()) < 0.001) {
      LOG_WARNING() << "estimated parameter '" << estimate->parameter() << "' was within 0.001 of lower bound " << estimate->lower_bound();
    } else if ((estimate->upper_bound() - estimate->value()) < 0.001) {
      LOG_WARNING() << "estimated parameter '" << estimate->parameter() << "' was within 0.001 of upper bound " << estimate->upper_bound();
    }
  }

  if (estimates.size() > 0) {
    if (format_ == PARAM_R) {
      cache_ << ReportHeader(type_, label_, format_);
      cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;
      for (Estimate* estimate : estimates) 
        cache_ << estimate->parameter() << " ";
 
      if(model->run_mode() == RunMode::kProfiling) {
        LOG_FINE() << "profileing";
        for (Profile* profile : profiles) 
          cache_ << profile->parameter() << " ";
        cache_ << REPORT_EOL;
      } else {
        cache_ << REPORT_EOL;
      }
      for (Estimate* estimate : estimates) 
        cache_ << AS_DOUBLE(estimate->value()) << " ";
      if(model->run_mode() == RunMode::kProfiling) {
        LOG_FINE() << "profileing";
        for (Profile* profile : profiles) {
          cache_ << AS_DOUBLE(profile->value()) << " ";
          LOG_FINE() << AS_DOUBLE(profile->value());
        }
        cache_ << REPORT_EOL;
      } else {
        cache_ << REPORT_EOL;
      }
    } else if (format_ == PARAM_NONE) {
      skip_tags_ = true;
      if (first_run_) {
        for (Estimate* estimate : estimates) 
          cache_ << estimate->parameter() << " ";
        if(model->run_mode() == RunMode::kProfiling) {
          LOG_FINE() << "profileing";
          for (Profile* profile : profiles) 
            cache_ << profile->parameter() << " ";
          cache_ << REPORT_EOL;
        } else {
          cache_ << REPORT_EOL;
        }
        first_run_ = false;
      }
      for (Estimate* estimate : estimates) 
        cache_ << AS_DOUBLE(estimate->value()) << " ";
      if(model->run_mode() == RunMode::kProfiling) {
        LOG_FINE() << "profileing";
        for (Profile* profile : profiles) {
          cache_ << AS_DOUBLE(profile->value()) << " ";
          LOG_FINE() << AS_DOUBLE(profile->value());
        }
        cache_ << REPORT_EOL;
      } else {
        cache_ << REPORT_EOL;
      }
    } else
      LOG_FATAL() << "Report format type (" << format_ << ") not known";

    /**auto minimiser_ = model->managers()->minimiser()->active_minimiser();
    if (minimiser_) {
      vector<Double> est_std_dev(estimates.size(), 0.0);
      covariance_matrix_ = minimiser_->covariance_matrix();
      if (model->run_mode() == RunMode::kEstimation && estimates.size() != covariance_matrix_.size1())
        LOG_WARNING() << "number of estimated parameters " << estimates.size() << " does not match the dimension of the covariance matrix "
          << covariance_matrix_.size1();
      if (covariance_matrix_.size1() > 0) {
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
    }**/
    // TODO: Fix this

    ready_for_writing_ = true;
  }
}

/**
 *  Execute the report in tabular format
 */
void EstimateValue::DoExecuteTabular(shared_ptr<Model> model) {
  vector<Estimate*> estimates = model->managers()->estimate()->objects();
  /**
   * if this is the first run we print the report header etc
   */
  if (first_run_) {
    first_run_ = false;
    cache_ << ReportHeader(type_, label_, format_);
    cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;
    for (Estimate* estimate : estimates) cache_ << estimate->parameter() << " ";
    cache_ << REPORT_EOL;
  }

  for (Estimate* estimate : estimates) cache_ << AS_DOUBLE(estimate->value()) << " ";
  cache_ << REPORT_EOL;

  if (estimates.size() > 0) {
    //    auto minimiser_ = model->managers()->minimiser()->active_minimiser();
    //    if (minimiser_) {
    //      covariance_matrix_ = minimiser_->covariance_matrix();
    //      if (model->run_mode() == RunMode::kEstimation && estimates.size() != covariance_matrix_.size1())
    //        LOG_WARNING() << "The number of estimated parameters " << estimates.size() << " does not match the dimension of the covariance matrix "
    //          << covariance_matrix_.size1();
    //      if (covariance_matrix_.size1() > 0) {
    //        vector<Double> est_std_dev(covariance_matrix_.size1(), 0.0);
    //        for (unsigned i = 0; i < covariance_matrix_.size1(); ++i)
    //          est_std_dev[i] = sqrt(covariance_matrix_(i, i));
    //
    //        for (auto sd: est_std_dev)
    //          cache_ << sd << " ";
    //        cache_ << "\n";
    //      }
    //    }
    // TODO: FIx this
  }
}

/**
 *  Finalise the tabular report
 */
void EstimateValue::DoFinaliseTabular(shared_ptr<Model> model) {
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
