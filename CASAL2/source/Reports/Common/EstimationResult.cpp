/**
 * @file EstimationResult.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 03/01/2019
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 */

// headers
#include "EstimationResult.h"

#include "../../Minimisers/Manager.h"
#include "../../Minimisers/Minimiser.h"
#include "../../Model/Managers.h"
#include "../../Model/Model.h"

// Namespaces
namespace niwa {
namespace reports {

/**
 * Default Constructor
 */
EstimationResult::EstimationResult() {
  run_mode_    = (RunMode::Type)(RunMode::kEstimation | RunMode::kProfiling);
  model_state_ = State::kIterationComplete;
}

/**
 * Execute the estimate summary report
 */
void EstimationResult::DoExecute(shared_ptr<Model> model) {
  auto minimiser = model->managers()->minimiser()->active_minimiser();
  if (minimiser == nullptr) {
    LOG_CODE_ERROR() << "minimiser == nullptr";
  }


  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "minimiser_values: ";
  auto est_vals = minimiser->get_estimated_values();
  for(auto val : est_vals)
    cache_ << val << " ";
  cache_ << REPORT_EOL;

  cache_ << PARAM_MINIMIZER << ": " << minimiser->label() << REPORT_EOL;
  cache_ << PARAM_TYPE << ": " << minimiser->type() << REPORT_EOL;
  cache_ << "Result " << REPORT_R_STRING_VECTOR << REPORT_EOL;
  switch (minimiser->result()) {
    case MinimiserResult::kInvalid:
      cache_ << "Failed" << REPORT_EOL;
      cache_ << "Message " << REPORT_R_STRING_VECTOR << REPORT_EOL;
      cache_ << "Invalid/No/Result/Code Error" << REPORT_EOL;
      break;
    case MinimiserResult::kSuccess:
      cache_ << "Success" << REPORT_EOL;
      cache_ << "Message " << REPORT_R_STRING_VECTOR << REPORT_EOL;
      cache_ << "Convergence" << REPORT_EOL;
      break;
    case MinimiserResult::kStepSizeTooSmallSuccess:
      cache_ << "Success" << REPORT_EOL;
      cache_ << "Message " << REPORT_R_STRING_VECTOR << REPORT_EOL;
      cache_ << "Convergence, Step size too small" << REPORT_EOL;
      break;
    case MinimiserResult::kError:
      cache_ << "Failed" << REPORT_EOL;
      cache_ << "Message " << REPORT_R_STRING_VECTOR << REPORT_EOL;
      cache_ << "Error" << REPORT_EOL;
      break;
    case MinimiserResult::kTooManyIterations:
      cache_ << "Failed" << REPORT_EOL;
      cache_ << "Message " << REPORT_R_STRING_VECTOR << REPORT_EOL;
      cache_ << "Too many iterations" << REPORT_EOL;
      break;
    case MinimiserResult::kTooManyEvaluations:
      cache_ << "Failed" << REPORT_EOL;
      cache_ << "Message " << REPORT_R_STRING_VECTOR << REPORT_EOL;
      cache_ << "Too many objective function evaluations" << REPORT_EOL;
      break;
    case MinimiserResult::kStepSizeTooSmall:
      cache_ << "Failed" << REPORT_EOL;
      cache_ << "Message " << REPORT_R_STRING_VECTOR << REPORT_EOL;
      cache_ << "Step size too small, no convergence" << REPORT_EOL;
      break;
    case MinimiserResult::kUnclearConvergence:
      cache_ << "Failed" << REPORT_EOL;
      cache_ << "Message " << REPORT_R_STRING_VECTOR << REPORT_EOL;
      cache_ << "Unclear convergence in optimise. May or may not be a local minimum - you need to investigate further." << REPORT_EOL;
      break;
    default:
      cache_ << "Failed" << REPORT_EOL;
      cache_ << "Message " << REPORT_R_STRING_VECTOR << REPORT_EOL;
      cache_ << "Unknown Error" << REPORT_EOL;
      break;
  }

  ready_for_writing_ = true;
}

void EstimationResult::DoPrepareTabular(shared_ptr<Model> model) {
  LOG_INFO() << "Tabular mode for reports of type " << PARAM_ESTIMATION_RESULT << " has not been implemented";
}

} /* namespace reports */
} /* namespace niwa */
