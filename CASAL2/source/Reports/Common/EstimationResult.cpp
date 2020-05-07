/**
 * @file EstimationResult.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 03/01/2019
 * @section LICENSE
 *
 * Copyright NIWA Science ©2019 - www.niwa.co.nz
 */

// headers
#include <Reports/Common/EstimationResult.h>
#include "Minimisers/Manager.h"
#include "Minimisers/Minimiser.h"
#include "Model/Managers.h"
#include "Model/Model.h"

// Namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
EstimationResult::EstimationResult(Model* model) : Report(model) {
  run_mode_    = RunMode::kEstimation;
  model_state_ = State::kIterationComplete;
}

/**
 * Destructor
 */
EstimationResult::~EstimationResult() noexcept(true) {
}

/**
 * Execute the estimate summary report
 */
void EstimationResult::DoExecute() {
  auto minimiser = model_->managers().minimiser()->active_minimiser();
  if (minimiser == nullptr) {
    LOG_CODE_ERROR() << "minimiser == nullptr";
  }

  cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
  cache_ << PARAM_MINIMIZER << ": " << minimiser->label() << "\n";
  cache_ << PARAM_TYPE << ": " << minimiser->type() << "\n";
  cache_ << "Result " << REPORT_R_STRING_VECTOR << "\n";
  switch (minimiser->result()) {
    case MinimiserResult::kInvalid:
      cache_ << "Failed\n";
      cache_ << "Message " << REPORT_R_STRING_VECTOR << "\n";
      cache_ << "Invalid/No/Result/Code Error\n";
      break;
    case MinimiserResult::kSuccess:
      cache_ << "Success\n";
      cache_ << "Message " << REPORT_R_STRING_VECTOR << "\n";
      cache_ << "Convergence\n";
      break;
    case MinimiserResult::kStepSizeTooSmallSuccess:
      cache_ << "Success\n";
      cache_ << "Message " << REPORT_R_STRING_VECTOR << "\n";
      cache_ << "Convergence, Step size too small\n";
      break;
    case MinimiserResult::kError:
      cache_ << "Failed\n";
      cache_ << "Message " << REPORT_R_STRING_VECTOR << "\n";
      cache_ << "Error\n";
      break;
    case MinimiserResult::kTooManyIterations:
      cache_ << "Failed\n";
      cache_ << "Message " << REPORT_R_STRING_VECTOR << "\n";
      cache_ << "Too many iterations\n";
      break;
    case MinimiserResult::kTooManyEvaluations:
      cache_ << "Failed\n";
      cache_ << "Message " << REPORT_R_STRING_VECTOR << "\n";
      cache_ << "Too many objective function evaluations\n";
      break;
    case MinimiserResult::kStepSizeTooSmall:
      cache_ << "Failed\n";
      cache_ << "Message " << REPORT_R_STRING_VECTOR << "\n";
      cache_ << "Step size too small, no convergence\n";
      break;
    default:
      cache_ << "Failed\n";
      cache_ << "Message " << REPORT_R_STRING_VECTOR << "\n";
      cache_ << "Unknown Error\n";
      break;
  }

  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
