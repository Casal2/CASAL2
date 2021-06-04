/**
 * @file PiApproximation.cpp
 * @author  Scott Rasmussen (scott@zaita.com)
 * @date 20/09/2019
 * @section LICENSE
 *
 * Copyright NIWA Science �2019 - www.niwa.co.nz
 */

// headers
#ifndef USE_AUTODIFF
#include "PiApproximation.h"

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
 *
 */
PiApproximation::PiApproximation() {
  partition_type_ = PartitionType::kPiApprox;
  size_           = 100.0;

  parameters_.Bind<bool>(PARAM_USE_RANDOM, &use_random_, "Use random numbers for points, not the defined MCMC algorithm", "", false);
}

/**
 *
 */
PiApproximation::~PiApproximation() {}

/**
 *
 */
bool PiApproximation::Start(RunMode::Type run_mode) {
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
 *
 */
void PiApproximation::DoValidate() {}

/**
 *
 */
void PiApproximation::FullIteration() {
  Reset();

  double r = size_ / 2;

  // Add New random point

  double new_x = 0.0;
  double new_y = 0.0;

  if (use_random_) {
    new_x = utilities::RandomNumberGenerator::Instance().uniform(0, size_);
    new_y = utilities::RandomNumberGenerator::Instance().uniform(0, size_);
  } else {
    new_x = managers()->selectivity()->GetSelectivity("X")->GetAgeResult(5, nullptr);
    new_y = managers()->selectivity()->GetSelectivity("Y")->GetAgeResult(5, nullptr);
  }

  // Calculate number of points in and out or cicle
  double distance = sqrt((new_x - r) * (new_x - r) + (new_y - r) * (new_y - r));
  // Calculate addressable

  if (distance < r)
    inside_++;
  else
    outside_++;

  double insided   = inside_ * 1.0;
  double outsided  = outside_ * 1.0;
  double pi_approx = 4 * (insided / (insided + outsided));
  objective_score_ = math::PI + abs(math::PI - pi_approx);

  managers()->penalty()->FlagPenalty("PI_Approximation", objective_score_);
  cout << "Objective Score: " << objective_score_ << " (" << (inside_ + outside_) << ") - "
       << "inside_ / outside_: " << inside_ << " : " << outside_ << "; new_x/new_y: " << new_x << "/" << new_y << endl;
}

} /* namespace model */
} /* namespace niwa */
#endif
