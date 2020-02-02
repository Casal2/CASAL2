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
 * Default Constructor
 */
EstimationResult::EstimationResult(Model* model) : Report(model) {
  run_mode_    = RunMode::kEstimation;
  model_state_  = State::kIterationComplete;
}

/**
 * Destructor
 */
EstimationResult::~EstimationResult() noexcept(true) {
}

/**
 * This method will execute our estimate summary report
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
    cache_ << "Invalid/No/Result/Code Error\n";
    break;
  case MinimiserResult::kSuccess:
    cache_ << "Estimation success (Convergence)\n";
    break;
  case MinimiserResult::kStepSizeTooSmallSuccess:
    cache_ << "Estimation success (Step size too small convergence)\n";
    break;
  case MinimiserResult::kError:
    cache_ << "Estimation failed (Error)\n";
    break;
  case MinimiserResult::kTooManyIterations:
    cache_ << "Estimation failed (Too many iterations)\n";
    break;
  case MinimiserResult::kTooManyEvaluations:
    cache_ << "Estimation failed (Too many objective evaluations)\n";
    break;
  case MinimiserResult::kStepSizeTooSmall:
    cache_ << "Estimation failed (Step size too small, cannot find convergence)\n";
    break;
  default:
    cache_ << "Estimation failed (Unknown Error)\n";
    break;
  }

  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
