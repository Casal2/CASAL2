/**
 * @file RandomWalkMetropolisHastings.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-04-24
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef USE_AUTODIFF

// headers
#include "RandomWalkMetropolisHastings.h"

#include "../../Estimates/Manager.h"
#include "../../Minimisers/Manager.h"
#include "../../Model/Model.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../Reports/Manager.h"
#include "../../Utilities/Math.h"
#include "../../Utilities/RandomNumberGenerator.h"

// namespaces
namespace niwa::mcmcs {

namespace math = niwa::utilities::math;

/**
 * @brief Construct a new Random Walk Metropolis Hastings:: Random Walk Metropolis Hastings object
 *
 * @param model
 */
RandomWalkMetropolisHastings::RandomWalkMetropolisHastings(shared_ptr<Model> model) : MCMC(model) {
  type_ = PARAM_RANDOMWALK;
}

/**
 * @brief
 *
 * @param thread_pool Threadpool for running model iterations
 */
void RandomWalkMetropolisHastings::DoExecute(shared_ptr<ThreadPool> thread_pool) {
  thread_pool->TerminateAll();

  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  LOG_MEDIUM() << "MCMC Starting";
  LOG_MEDIUM() << "Covariance matrix has rows = " << covariance_matrix_.size1() << " and cols = " << covariance_matrix_.size2();
  LOG_MEDIUM() << "Estimate Count: " << estimate_count_;

  vector<double>     current_candidates = candidates_;
  ObjectiveFunction& obj_function       = model_->objective_function();
  double             current_score      = obj_function.score();

  mcmc::ChainLink last_link;

  double proposed_score            = 0.0;
  double current_likelihoods       = 0.0;
  double current_priors            = 0.0;
  double current_penalties         = 0.0;
  double current_additional_priors = 0.0;
  double current_jacobians         = 0.0;
  int    invalid_counter           = 0;

  do {
    // std::cout << ".";
    // if (jumps_ % 150 == 0)
    // std::cout << std::endl;

    UpdateStepSize();
    UpdateCovarianceMatrix();
    GenerateNewCandidates();

    if (WithinBounds()) {
      ++jumps_;
      ++jumps_since_adapt_;

      for (unsigned i = 0; i < candidates_.size(); ++i) {
        // only change values for estimates not fixed.
        if(!estimates_[i]->mcmc_fixed())
          estimates_[i]->set_value(candidates_[i]);
      }

      model_->FullIteration();
      obj_function.CalculateScore();
      proposed_score = obj_function.score();

      Double ratio = 1.0;
      if (proposed_score >= current_score) {
        ratio = exp(current_score - proposed_score);
      }

      // Check if we accept this jump
      // double rng_uniform = rng.uniform();
      if (math::IsEqual(ratio, 1.0) || rng.uniform() < ratio) {
        LOG_MEDIUM() << "Accept: Possible. Iteration = " << jumps_ << ", score = " << proposed_score << " current score " << current_score;
        successful_jumps_++;
        successful_jumps_since_adapt_++;
        current_score             = proposed_score;
        current_candidates        = candidates_;
        current_likelihoods       = obj_function.likelihoods();
        current_priors            = obj_function.priors();
        current_penalties         = obj_function.penalties();
        current_additional_priors = obj_function.additional_priors();
        current_jacobians         = obj_function.jacobians();

      } else {  // reject this jump reset
        candidates_ = current_candidates;
        LOG_MEDIUM() << "Reject: Possible. Iteration = " << jumps_ << ", score = " << proposed_score << " current score " << current_score;
      }

      if (jumps_ % keep_ == 0) {
        if (jumps_ > burn_in_)
          mcmc_state_ = PARAM_MCMC;
        else
          mcmc_state_ = PARAM_BURN_IN;
        mcmc::ChainLink new_link{.iteration_                   = jumps_,
                                 .mcmc_state_                  = mcmc_state_,
                                 .score_                       = current_score,
                                 .likelihood_                  = current_likelihoods,
                                 .prior_                       = current_priors,
                                 .penalty_                     = current_penalties,
                                 .additional_priors_           = current_additional_priors,
                                 .jacobians_                   = current_jacobians,
                                 .acceptance_rate_             = double(successful_jumps_) / double(jumps_),
                                 .acceptance_rate_since_adapt_ = double(successful_jumps_since_adapt_) / double(jumps_since_adapt_),
                                 .step_size_                   = step_size_,
                                 .values_                      = candidates_};

        chain_.push_back(new_link);
        // LOG_MEDIUM() << "Storing: Successful Jumps " << successful_jumps_ << " Jumps : " << jumps_;
        model_->managers()->report()->Execute(model_, State::kIterationComplete);
      }
    } else {  // Attempt was out of bounds, and hence reset to last in-bounds candidate, as used by GenerateNewCandidates
      ++invalid_counter;
      candidates_ = current_candidates;
      LOG_MEDIUM() << "Reject: Bounds. Iteration = " << jumps_ << ", score = " << proposed_score << " current score " << current_score << ". There were " << invalid_counter
                   << " invalid jumps";
    }
  } while (jumps_ < length_);
}

}  // namespace niwa::mcmcs
#endif  // USE_AUTODIFF