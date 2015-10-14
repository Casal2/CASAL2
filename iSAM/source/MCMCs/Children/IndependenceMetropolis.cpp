/**
 * @file IndependenceMetropolis.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/05/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "IndependenceMetropolis.h"

#include "Estimates/Manager.h"
#include "Model/Model.h"
#include "Minimisers/Manager.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Reports/Manager.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/RandomNumberGenerator.h"

// namespaces
namespace niwa {
namespace mcmcs {

namespace dc = niwa::utilities::doublecompare;

/**
 * Default constructor
 */
IndependenceMetropolis::IndependenceMetropolis(Model* model) : MCMC(model) {
}


/**
 * Execute the MCMC system and build our MCMC chain
 */
void IndependenceMetropolis::Execute() {
  candidates_.resize(estimate_count_);
  is_enabled_estimate_.resize(estimate_count_);

  vector<Estimate*> estimates = model_->managers().estimate()->GetEnabled();
  for (unsigned i = 0; i < estimate_count_; ++i) {
    candidates_[i] = AS_DOUBLE(estimates[i]->value());

    if (estimates[i]->lower_bound() == estimates[i]->upper_bound() || estimates[i]->mcmc_fixed())
      is_enabled_estimate_[i] = false;
    else
      is_enabled_estimate_[i] = true;
  }

  BuildCovarianceMatrix();

  if (!DoCholeskyDecmposition())
    LOG_FATAL() << "Cholesky decomposition failed. Cannot continue MCMC";

  if (start_ > 0.0)
    GenerateRandomStart();

  for(unsigned i = 0; i < estimate_count_; ++i)
    estimates[i]->set_value(candidates_[i]);

  /**
   * Get the objective score
   */
  model_->FullIteration();
  ObjectiveFunction& obj_function = model_->objective_function();
  obj_function.CalculateScore();
  Double score = AS_DOUBLE(obj_function.score());

  /**
   * Store first location
   */
  {
    mcmc::ChainLink new_link;
    new_link.iteration_                     = 0;
    new_link.penalty_                       = AS_DOUBLE(obj_function.penalties());
    new_link.score_                         = AS_DOUBLE(obj_function.score());
    new_link.prior_                         = AS_DOUBLE(obj_function.priors());
    new_link.likelihood_                    = AS_DOUBLE(obj_function.likelihoods());
    new_link.additional_priors_             = AS_DOUBLE(obj_function.additional_priors());
    new_link.acceptance_rate_               = 0;
    new_link.acceptance_rate_since_adapt_   = 0;
    new_link.step_size_                     = step_size_;
    new_link.values_                        = candidates_;
    chain_.push_back(new_link);
  }

  /**
   * Now we start the MCMC process
   */
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  do {
    LOG_FINE()<<"MCMC Starting";

    vector<Double> original_candidates = candidates_;
    UpdateStepSize();
    GenerateNewCandidates();
    for (unsigned i = 0; i < candidates_.size(); ++i)
      estimates[i]->set_value(candidates_[i]);

    model_->FullIteration();
    obj_function.CalculateScore();

    Double previous_score = score;
    score = AS_DOUBLE(obj_function.score());
    Double ratio = 1.0;

    if (score > previous_score) {
      ratio = exp(-score + previous_score);
      LOG_MEDIUM() << " Current Objective score " << score << " Previous score " << previous_score;
    }

    if (dc::IsEqual(ratio, 1.0) || rng.uniform() < ratio) {
      LOG_MEDIUM() << "Ratio = " << ratio << " random_number = " << rng.uniform();
      // Accept this jump
      successful_jumps_++;
      successful_jumps_since_adapt_++;
      jumps_++;
      jumps_since_adapt_++;
      // keep the score, and its compontent parts if the following condition holds
      if (successful_jumps_ % keep_ == 0) {
        LOG_FINE() << "Keeping jump " << successful_jumps_;
        mcmc::ChainLink new_link;
        new_link.iteration_ = successful_jumps_;
        new_link.penalty_ = obj_function.penalties();
        new_link.score_ = AS_DOUBLE(obj_function.score());
        new_link.prior_ = obj_function.priors();
        new_link.likelihood_ = obj_function.likelihoods();
        new_link.additional_priors_ = obj_function.additional_priors();
        new_link.acceptance_rate_ = Double(successful_jumps_) / Double(jumps_);
        new_link.acceptance_rate_since_adapt_ = Double(successful_jumps_since_adapt_) / Double(jumps_since_adapt_);
        new_link.step_size_ = step_size_;
        new_link.values_ = candidates_;
        chain_.push_back(new_link);
        LOG_MEDIUM() << "Successful Jumps " << successful_jumps_ << " Jumps : " << jumps_ << " successful jumps since adapt " << successful_jumps_since_adapt_
            << " jumps since adapt " << jumps_since_adapt_;
        model_->managers().report()->Execute(State::kIterationComplete);
      }
    } else {
      // Reject this attempt
      score = previous_score;
      candidates_ = original_candidates;
      jumps_++;
      jumps_since_adapt_++;
    }

    LOG_FINEST() << successful_jumps_ << " successful jumps have been completed";
  } while (successful_jumps_ < length_);
}

} /* namespace mcmcs */
} /* namespace niwa */
