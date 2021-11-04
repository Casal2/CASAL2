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
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  LOG_MEDIUM() << "MCMC Starting";
  LOG_MEDIUM() << "Covariance matrix has rows = " << covariance_matrix_.size1() << " and cols = " << covariance_matrix_.size2();
  LOG_MEDIUM() << "Estimate Count: " << estimate_count_;

  vector<double>     previous_candidates = candidates_;
  ObjectiveFunction& obj_function        = model_->objective_function();
  double             previous_score      = obj_function.score();

  mcmc::ChainLink last_link;
  do {
    // std::cout << ".";
    // if (jumps_ % 150 == 0)
    // std::cout << std::endl;

    UpdateStepSize();
    UpdateCovarianceMatrix();
    GenerateNewCandidates();

    ++jumps_;
    ++jumps_since_adapt_;
    bool   accept_jump = false;
    double score       = 0.0;

    if (WithinBounds()) {
      for (unsigned i = 0; i < candidates_.size(); ++i) estimates_[i]->set_value(candidates_[i]);

      model_->FullIteration();
      obj_function.CalculateScore();
      score = obj_function.score();

      Double ratio = 1.0;
      if (score >= previous_score) {
        ratio = exp(previous_score - score);
      }

      // Check if we accept this jump
      // double rng_uniform = rng.uniform();
      if (math::IsEqual(ratio, 1.0) || rng.uniform() < ratio) {
        LOG_MEDIUM() << "Accept: Possible. Iteration = " << jumps_ << ", score = " << score << " Previous score " << previous_score;
        // Accept this jump
        accept_jump = true;
        successful_jumps_++;
        successful_jumps_since_adapt_++;
        previous_score      = score;
        previous_candidates = candidates_;
      } else {  // reject this jump reset
        candidates_ = previous_candidates;
        LOG_MEDIUM() << "Reject: Possible. Iteration = " << jumps_ << ", score = " << score << " Previous score " << previous_score;
      }
    } else {  // Reject this attempt but still record the chain if it lands on a keep
      LOG_MEDIUM() << "Reject: Bounds. Iteration = " << jumps_ << ", score = " << score << " Previous score " << previous_score;
      candidates_ = previous_candidates;
    }

    if (jumps_ % keep_ == 0) {
      if (jumps_ > burn_in_)
        mcmc_state_ = PARAM_MCMC;
      else
        mcmc_state_ = PARAM_BURN_IN;
      if (accept_jump) {
        mcmc::ChainLink new_link{.iteration_                   = jumps_,
                                 .mcmc_state_                  = mcmc_state_,
                                 .score_                       = obj_function.score(),
                                 .likelihood_                  = obj_function.likelihoods(),
                                 .prior_                       = obj_function.priors(),
                                 .penalty_                     = obj_function.penalties(),
                                 .additional_priors_           = obj_function.additional_priors(),
                                 .jacobians_                   = obj_function.jacobians(),
                                 .acceptance_rate_             = double(successful_jumps_) / double(jumps_),
                                 .acceptance_rate_since_adapt_ = double(successful_jumps_since_adapt_) / double(jumps_since_adapt_),
                                 .step_size_                   = step_size_,
                                 .values_                      = candidates_};

        chain_.push_back(new_link);
      } else {
        // Copy the last chain accepted link to the end of the vector
        auto temp                         = *chain_.rbegin();
        temp.iteration_                   = jumps_;
        temp.mcmc_state_                  = mcmc_state_;
        temp.acceptance_rate_             = double(successful_jumps_) / double(jumps_);
        temp.acceptance_rate_since_adapt_ = double(successful_jumps_since_adapt_) / double(jumps_since_adapt_);
        chain_.push_back(temp);
      }

      // print out a . for every iteration so we get some feedback to console
      // ++iteration_counter_;
      // cout << ".";
      // if (iteration_counter_ % 100 == 0) {
      //   iteration_counter_ = 0;
      //   cout << endl;
      // }

      // LOG_MEDIUM() << "Storing: Successful Jumps " << successful_jumps_ << " Jumps : " << jumps_;
      model_->managers()->report()->Execute(model_, State::kIterationComplete);
    }
  } while (jumps_ < length_);
}

}  // namespace niwa::mcmcs
#endif  // USE_AUTODIFF