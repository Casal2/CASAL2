/**
 * @file Multivariate.cpp
 * @author  Scott Rasmussen (scott@zaita.com)
 * @date 20/09/2019
 * @section LICENSE
 *
 * Copyright NIWA Science �2019 - www.niwa.co.nz
 */
#ifndef USE_AUTODIFF
// headers
#include "Multivariate.h"

// #include <Eigen/Eigen>

#include "../../Estimates/Manager.h"
#include "../../MCMCs/MCMC.h"
#include "../../MCMCs/Manager.h"
#include "../../Model/Managers.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../Penalties/Manager.h"
#include "../../Reports/Manager.h"
#include "../../Selectivities/Manager.h"
#include "../../Utilities/Math.h"
#include "../../Utilities/RandomNumberGenerator.h"

// namespaces
namespace niwa {
namespace model {

namespace math = niwa::utilities::math;

/**
 * Default constructor
 */
Multivariate::Multivariate() {
  partition_type_ = PartitionType::kMultivariate;

  parameters_.Bind<bool>(PARAM_USE_RANDOM, &use_random_, "Use random numbers for points, not the defined MCMC algorithm", "", false);

  // Mean should be 0.0, 0.0
  // Cov  should be [[ 1.0, 0.0]][ 0. 1.0] // Identity Matrix
}

/**
 * Start our model. This handles the different
 * run_modes
 *
 * @param run_mode Run mode model has been started with
 */
bool Multivariate::Start(RunMode::Type run_mode) {
  run_mode_ = run_mode;

  RegisterAsAddressable(PARAM_A, &objective_score_);

  managers_->mcmc()->Validate(pointer());
  managers_->report()->Validate(pointer());
  managers_->selectivity()->Validate();
  managers_->estimate()->Validate(pointer());

  managers_->report()->Build(pointer());
  managers_->selectivity()->Build();
  managers_->estimate()->Build(pointer());

  if (run_mode == RunMode::kBasic)
    FullIteration();
  else if (run_mode == RunMode::kMCMC) {
    managers_->mcmc()->Build();

    // auto mcmc = managers_->mcmc()->active_mcmc();
    // mcmc->Execute();
  } else {
    LOG_CODE_ERROR() << "RunMode not supported";
  }

  objective_function_->CalculateScore();
  managers_->report()->Execute(pointer(), State::kIterationComplete);
  return true;
}

/**
 * @brief
 *
 */
void Multivariate::FullIteration() {
  LOG_CODE_ERROR() << "The multivariate model has not yet been implemented";
}

} /* namespace model */
} /* namespace niwa */
#endif
