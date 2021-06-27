/**
 * @file IndependenceMetropolis.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/05/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */
#ifndef USE_AUTODIFF

// headers
#include "IndependenceMetropolis.h"

#include "../../EstimateTransformations/Manager.h"
#include "../../Estimates/Manager.h"
#include "../../Minimisers/Manager.h"
#include "../../Model/Model.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../Reports/Manager.h"
#include "../../Utilities/Math.h"
#include "../../Utilities/RandomNumberGenerator.h"

// namespaces
namespace niwa {
namespace mcmcs {

namespace math = niwa::utilities::math;

/**
 * Default constructor
 */
IndependenceMetropolis::IndependenceMetropolis(shared_ptr<Model> model) : MCMC(model) {
  type_ = PARAM_INDEPENDENCE_METROPOLIS;
}

/**
 * Execute the MCMC system and build the MCMC chain
 */
void IndependenceMetropolis::DoExecute(shared_ptr<ThreadPool> thread_pool) {
  vector<Double> previous_untransformed_candidates = candidates_;

  // Transform any parameters so that candidates are in the same space as the covariance matrix.
  model_->managers()->estimate_transformation()->TransformEstimatesForObjectiveFunction();
  for (unsigned i = 0; i < estimate_count_; ++i) {
    candidates_[i] = estimates_[i]->value();
  }

  if (!model_->global_configuration().resume_mcmc()) {
    LOG_MEDIUM() << "Not resuming";
    BuildCovarianceMatrix();
    successful_jumps_ = starting_iteration_;
  }

  // Set jumps = starting iteration if it is resuming
  unsigned jumps_since_last_adapt = 1;
  if (model_->global_configuration().resume_mcmc()) {
    for (unsigned i = 0; i < adapt_step_size_.size(); ++i) {
      if (adapt_step_size_[i] < starting_iteration_) {
        jumps_since_last_adapt = adapt_step_size_[i];
        LOG_MEDIUM() << "Chain last adapted at " << jumps_since_last_adapt;
      }
    }
    jumps_             = starting_iteration_;
    jumps_since_adapt_ = jumps_ - jumps_since_last_adapt;

    double temp_success_jumps     = (double)jumps_since_adapt_ * acceptance_rate_since_last_adapt_;
    successful_jumps_since_adapt_ = (unsigned)temp_success_jumps;

    // if (!utilities::To<double, unsigned>(temp_success_jumps, successful_jumps_since_adapt_))
    // LOG_ERROR() << "Could not convert " << temp_success_jumps << " to an unsigned integer";

    LOG_FINE() << "jumps = " << jumps_ << "jumps since last adapt " << jumps_since_adapt_ << " successful jumps since last adapt " << successful_jumps_since_adapt_ << " step size "
               << step_size_ << " successful jumps " << successful_jumps_;
  }

  LOG_MEDIUM() << "Applying Cholesky decomposition";
  if (!DoCholeskyDecomposition())
    LOG_FATAL() << "Cholesky decomposition failed. Cannot continue MCMC";

  if (start_ > 0.0) {
    // Take into account any transformations so that when we compare with bounds we are in correct space, when: prior_applies_to_transform true
    GenerateRandomStart();
  }

  for (unsigned i = 0; i < estimate_count_; ++i) estimates_[i]->set_value(candidates_[i]);

  /**
   * Get the objective score
   */
  // Do a quick restore so that estimates are in a space the model wants
  model_->managers()->estimate_transformation()->RestoreEstimatesFromObjectiveFunction();
  model_->FullIteration();

  // For reporting purposes
  for (unsigned i = 0; i < estimate_count_; ++i) {
    previous_untransformed_candidates[i] = estimates_[i]->value();
  }

  ObjectiveFunction& obj_function = model_->objective_function();
  obj_function.CalculateScore();

  Double score            = AS_DOUBLE(obj_function.score());
  Double penalty          = AS_DOUBLE(obj_function.penalties());
  Double prior            = AS_DOUBLE(obj_function.priors());
  Double likelihood       = AS_DOUBLE(obj_function.likelihoods());
  Double additional_prior = AS_DOUBLE(obj_function.additional_priors());
  Double jacobian         = AS_DOUBLE(obj_function.jacobians());

  /**
   * Store first location
   */
  mcmc::ChainLink new_link;
  new_link.penalty_           = AS_DOUBLE(obj_function.penalties());
  new_link.score_             = AS_DOUBLE(obj_function.score());
  new_link.prior_             = AS_DOUBLE(obj_function.priors());
  new_link.likelihood_        = AS_DOUBLE(obj_function.likelihoods());
  new_link.additional_priors_ = AS_DOUBLE(obj_function.additional_priors());
  new_link.jacobians_         = AS_DOUBLE(obj_function.jacobians());
  new_link.step_size_         = step_size_;
  new_link.values_            = previous_untransformed_candidates;

  if (!model_->global_configuration().resume_mcmc()) {
    jumps_++;
    jumps_since_adapt_++;

    new_link.iteration_                   = jumps_;
    new_link.acceptance_rate_             = 0;
    new_link.acceptance_rate_since_adapt_ = 0;

    chain_.push_back(new_link);

    // Print first value
    model_->managers()->report()->Execute(model_, State::kIterationComplete);
  } else {
    // resume
    new_link.iteration_                   = jumps_;
    new_link.acceptance_rate_             = acceptance_rate_;
    new_link.acceptance_rate_since_adapt_ = acceptance_rate_since_last_adapt_;

    chain_.push_back(new_link);

    LOG_MEDIUM() << "Resuming MCMC chain with iteration " << jumps_;
  }

  /**
   * Now we start the MCMC process
   */
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  LOG_MEDIUM() << "MCMC Starting";
  LOG_MEDIUM() << "Covariance matrix has rows = " << covariance_matrix_.size1() << " and cols = " << covariance_matrix_.size2();
  LOG_MEDIUM() << "Estimate Count: " << estimate_count_;

  vector<Double> previous_candidates = candidates_;

  Double previous_score            = score;
  Double previous_prior            = prior;
  Double previous_likelihood       = likelihood;
  Double previous_penalty          = penalty;
  Double previous_additional_prior = additional_prior;
  Double previous_jacobian         = jacobian;

  do {
    // Check If we need to update the step size
    UpdateStepSize();

    // Check If we need to update the covariance
    UpdateCovarianceMatrix();

    // Generate new candidates
    // Need to make sure estimates are in the correct space.
    model_->managers()->estimate_transformation()->TransformEstimatesForObjectiveFunction();
    GenerateNewCandidates();

    // Count the jump
    jumps_++;
    jumps_since_adapt_++;

    // Check candidates are within the bounds.
    if (WithinBounds()) {
      // Trial these Potential candidates.
      for (unsigned i = 0; i < candidates_.size(); ++i) estimates_[i]->set_value(candidates_[i]);

      // restore for model run.
      model_->managers()->estimate_transformation()->RestoreEstimatesFromObjectiveFunction();

      // Run model with candidate parameters.
      model_->FullIteration();
      // evaluate objective score.
      obj_function.CalculateScore();

      // Store objective information if we accept these will become our current step
      score            = AS_DOUBLE(obj_function.score());
      penalty          = AS_DOUBLE(obj_function.penalties());
      prior            = AS_DOUBLE(obj_function.priors());
      likelihood       = AS_DOUBLE(obj_function.likelihoods());
      additional_prior = AS_DOUBLE(obj_function.additional_priors());
      jacobian         = AS_DOUBLE(obj_function.jacobians());

      Double ratio = 1.0;

      if (score >= previous_score) {
        ratio = exp(previous_score - score);
      }

      // Check if we accept this jump
      // double uniform = rng.uniform();
      if (math::IsEqual(ratio, 1.0) || rng.uniform() < ratio) {
        LOG_MEDIUM() << "Accept: Possible. Iteration = " << jumps_ << ", score = " << score << " Previous score " << previous_score;
        // Accept this jump
        successful_jumps_++;
        successful_jumps_since_adapt_++;

        // So these become our last step values so save them.
        previous_candidates       = candidates_;
        previous_score            = score;
        previous_prior            = prior;
        previous_likelihood       = likelihood;
        previous_penalty          = penalty;
        previous_additional_prior = additional_prior;
        previous_jacobian         = jacobian;

        // For reporting purposes
        for (unsigned i = 0; i < estimate_count_; ++i) {
          previous_untransformed_candidates[i] = estimates_[i]->value();
        }
      } else {
        // reject this jump reset
        candidates_ = previous_candidates;

        LOG_MEDIUM() << "Reject: Possible. Iteration = " << jumps_ << ", score = " << score << " Previous score " << previous_score;
      }
    } else {
      LOG_MEDIUM() << "Reject: Bounds. Iteration = " << jumps_ << ", score = " << score << " Previous score " << previous_score;
      // Reject this attempt but still record the chain if it lands on a keep
      candidates_ = previous_candidates;
    }

    if (jumps_ % keep_ == 0) {
      // Record the score, and its component parts if the successful jump divided by keep has no remainder
      // i.e this proposed candidate is a 'keep' iteration
      mcmc::ChainLink new_link;

      new_link.iteration_                   = jumps_;
      new_link.penalty_                     = previous_penalty;
      new_link.score_                       = previous_score;
      new_link.prior_                       = previous_prior;
      new_link.likelihood_                  = previous_likelihood;
      new_link.additional_priors_           = previous_additional_prior;
      new_link.jacobians_                   = previous_jacobian;
      new_link.acceptance_rate_             = double(successful_jumps_) / double(jumps_);
      new_link.acceptance_rate_since_adapt_ = double(successful_jumps_since_adapt_) / double(jumps_since_adapt_);
      new_link.step_size_                   = step_size_;
      new_link.values_                      = previous_untransformed_candidates;

      chain_.push_back(new_link);

      // LOG_MEDIUM() << "Storing: Successful Jumps " << successful_jumps_ << " Jumps : " << jumps_;
      model_->managers()->report()->Execute(model_, State::kIterationComplete);
    }
  } while (jumps_ < length_);
}

} /* namespace mcmcs */
} /* namespace niwa */
#endif  // USE_AUTODIFF