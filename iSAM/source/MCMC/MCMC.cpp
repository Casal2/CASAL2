/**
 * @file MCMC.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 10/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "MCMC.h"

#include "Estimates/Manager.h"
#include "Model/Model.h"
#include "Minimisers/Manager.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Reports/Manager.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/RandomNumberGenerator.h"


// namespaces
namespace isam {

namespace dc = isam::utilities::doublecompare;

/**
 * Constructor
 */
MCMC::MCMC() {
  parameters_.Bind<Double>(PARAM_START, &start_, "TBA", "", 0.0);
  parameters_.Bind<unsigned>(PARAM_LENGTH, &length_, "The number of chain links to create", "");
  parameters_.Bind<unsigned>(PARAM_KEEP, &keep_, "TBA", "", 1u);
  parameters_.Bind<Double>(PARAM_MAX_CORRELATION, &max_correlation_, "TBA", "", 0.8);
  parameters_.Bind<string>(PARAM_COVARIANCE_ADJUSTMENT_METHOD, &correlation_method_, "TBA", "", PARAM_COVARIANCE);
  parameters_.Bind<Double>(PARAM_CORRELATION_ADJUSTMENT_DIFF, &correlation_diff_, "TBA", "", 0.0001);
  parameters_.Bind<Double>(PARAM_STEP_SIZE, &step_size_, "TBA", "", 0.0);
  parameters_.Bind<string>(PARAM_PROPOSAL_DISTRIBUTION, &proposal_distribution_, "TBA", "", PARAM_T);
  parameters_.Bind<unsigned>(PARAM_DF, &df_, "TBA", "", 4);
  parameters_.Bind<unsigned>(PARAM_ADAPT_STEPSIZE_AT, &adapt_step_size_, "TBA", "", true);

  jumps_                          = 0;
  successful_jumps_               = 0;
  jumps_since_adapt_              = 0;
  successful_jumps_since_adapt_   = 0;
  step_size_                      = 0.0;
  last_item_                      = false;
}

/**
 * Our singleton accessor method
 *
 * @return singleton shared ptr
 */
shared_ptr<MCMC> MCMC::Instance() {
  static MCMCPtr mcmc = MCMCPtr(new MCMC());
  return mcmc;
}

/**
 * Validate the parameters defined in the configuration file
 */
void MCMC::Validate() {
  parameters_.Populate();

  if (adapt_step_size_.size() == 0)
    adapt_step_size_.assign(1, 1u);

  if (length_ <= 0)
    LOG_ERROR(parameters_.location(PARAM_LENGTH) << "(" << length_ << ") cannot be less than or equal to 0");

  for (unsigned adapt : adapt_step_size_) {
    if (adapt < 1)
      LOG_ERROR(parameters_.location(PARAM_ADAPT_STEPSIZE_AT) << "(" << adapt << ") cannot be less than 1");
    if (adapt > length_)
      LOG_ERROR(parameters_.location(PARAM_ADAPT_STEPSIZE_AT) << "(" << adapt << ") cannot be greater than length(" << length_ << ")");
  }

  if (correlation_method_ != PARAM_CORRELATION && correlation_method_ != PARAM_COVARIANCE && correlation_method_ != PARAM_NONE)
    LOG_ERROR(parameters_.location(PARAM_COVARIANCE_ADJUSTMENT_METHOD) << "(" << correlation_method_ << ")"
        << " is not supported. Currently supported values are " << PARAM_CORRELATION << ", " << PARAM_COVARIANCE << " and " << PARAM_NONE);

  if (proposal_distribution_ != PARAM_T && proposal_distribution_ != PARAM_NORMAL)
    LOG_ERROR(parameters_.location(PARAM_PROPOSAL_DISTRIBUTION) << "(" << proposal_distribution_ << ")"
        << " is not supported. Currently supported values are " << PARAM_T << " and " << PARAM_NORMAL);

  if (max_correlation_ <= 0.0 || max_correlation_ > 1.0)
    LOG_ERROR(parameters_.location(PARAM_MAX_CORRELATION) << "(" << max_correlation_ << ") must be between 0.0 (not inclusive) and 1.0 (inclusive)");
  if (df_ <= 0)
    LOG_ERROR(parameters_.location(PARAM_DF) << "(" << df_ << ") cannot be less or equal to 0");
  if (start_ < 0.0)
    LOG_ERROR(parameters_.location(PARAM_START) << "(" << start_ << ") cannot be less than 0");
  if (step_size_ < 0.0)
    LOG_ERROR(parameters_.location(PARAM_STEP_SIZE) << "(" << step_size_ << ") cannot be less than 0.0");
}

/**
 * Build any relationships we need and objects that will hold data
 * so they can be used during the execution
 */
void MCMC::Build() {

  minimiser_ = minimisers::Manager::Instance().active_minimiser();

  unsigned active_estimates = 0;

  vector<EstimatePtr> estimates = estimates::Manager::Instance().GetEnabled();
  estimate_count_ = estimates.size();
  for (EstimatePtr estimate : estimates) {
    estimate_labels_.push_back(estimate->label());

    if (estimate->upper_bound() == estimate->lower_bound() || estimate->mcmc_fixed())
      continue;
    active_estimates++;
  }

  if (active_estimates == 0)
    LOG_ERROR("While building the MCMC system the number of active estimates was 0. You need at least 1 non-fixed MCMC estimate");

  if (step_size_ == 0.0)
    step_size_ = 2.4 * pow((Double)active_estimates, -0.5);

}

/**
 * Execute the MCMC system and build our MCMC chain
 */
void MCMC::Execute() {
  candidates_.resize(estimate_count_);
  is_enabled_estimate_.resize(estimate_count_);

  vector<EstimatePtr> estimates = estimates::Manager::Instance().GetEnabled();
  for (unsigned i = 0; i < estimate_count_; ++i) {
    candidates_[i] = AS_DOUBLE(estimates[i]->value());

    if (estimates[i]->lower_bound() == estimates[i]->upper_bound() || estimates[i]->mcmc_fixed())
      is_enabled_estimate_[i] = false;
    else
      is_enabled_estimate_[i] = true;
  }

  BuildCovarianceMatrix();
  if (!DoCholeskyDecmposition())
    LOG_ERROR("Cholesky decomposition failed. Cannot continue MCMC");

  if (start_ > 0.0)
    GenerateRandomStart();

  for(unsigned i = 0; i < estimate_count_; ++i)
    estimates[i]->set_value(candidates_[i]);

  /**
   * Get the objective score
   */
  Model::Instance()->FullIteration();
  ObjectiveFunction obj_function = ObjectiveFunction::Instance();
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
    LOG_INFO("MCMC Starting");

    vector<Double> original_candidates = candidates_;
    UpdateStepSize();
    GenerateNewCandidates();
    for (unsigned i = 0; i < candidates_.size(); ++i)
      estimates[i]->set_value(candidates_[i]);

    Model::Instance()->FullIteration();
    obj_function.CalculateScore();

    Double previous_score = score;
    score = AS_DOUBLE(obj_function.score());
    Double ratio = 1.0;

    if (score > previous_score)
      ratio = exp(-score + previous_score);

    jumps_++;
    jumps_since_adapt_++;

    if (dc::IsEqual(ratio, 1.0) || rng.uniform() < ratio) {
      // Accept this jump
      successful_jumps_++;
      successful_jumps_since_adapt_++;

      // See if we need to check this a link we keep
      if (successful_jumps_ % keep_ == 0) {
        LOG_INFO("Keeping jump " << successful_jumps_);
        mcmc::ChainLink new_link;
        new_link.iteration_                     = successful_jumps_;
        new_link.penalty_                       = obj_function.penalties();
        new_link.score_                         = AS_DOUBLE(obj_function.score());
        new_link.prior_                         = obj_function.priors();
        new_link.likelihood_                    = obj_function.likelihoods();
        new_link.additional_priors_             = obj_function.additional_priors();
        new_link.acceptance_rate_               = successful_jumps_since_adapt_ / jumps_since_adapt_;
        new_link.acceptance_rate_since_adapt_   = successful_jumps_ / jumps_;
        new_link.step_size_                     = step_size_;
        new_link.values_                        = candidates_;
        chain_.push_back(new_link);

        reports::Manager::Instance().Execute(State::kIterationComplete);
      } else {
        // Reject this attempt
        score = previous_score;
        candidates_ = original_candidates;
      }
    }

  } while (successful_jumps_ < length_);
}

/**
 * Get the covariance matrix from the minimiser and then
 * adjust it for our proposal distribution
 */
void MCMC::BuildCovarianceMatrix() {
  covariance_matrix_ = minimiser_->covariance_matrix();
  if (correlation_method_ == PARAM_NONE)
    return;

  /**
   * Adjust the covariance matrix for the proposal distribution
   */
  for (unsigned i = 0; i < covariance_matrix_.size1(); ++i) {
    for (unsigned j = 0; j < covariance_matrix_.size2(); ++j) {
      if (covariance_matrix_(i,j) / sqrt(covariance_matrix_(i,i) * covariance_matrix_(j,j)) > max_correlation_) {
        covariance_matrix_(i,j) = max_correlation_ * sqrt(covariance_matrix_(i,i) * covariance_matrix_(j,j));
      }
      if (covariance_matrix_(i,j) / sqrt(covariance_matrix_(i,i) * covariance_matrix_(j,j)) < -max_correlation_){
        covariance_matrix_(i,j) = -max_correlation_ * sqrt(covariance_matrix_(i,i) * covariance_matrix_(j,j));
      }
    }
  }

  /**
   * Adjust any non-zero variances less than min_diff_ * difference between bounds
   */
  vector<Double> difference_bounds;
  vector<EstimatePtr> estimates = estimates::Manager::Instance().GetEnabled();
  for (EstimatePtr estimate : estimates) {
    difference_bounds.push_back( estimate->upper_bound() - estimate->lower_bound() );
  }

  for (unsigned i = 0; i < covariance_matrix_.size1(); ++i) {
    if (covariance_matrix_(i,i) < (correlation_diff_ * difference_bounds[i]) && covariance_matrix_(i,i) != 0) {
      if (correlation_method_ == PARAM_COVARIANCE) {
        Double multiply_covariance = (sqrt(correlation_diff_) * difference_bounds[i]) / sqrt(covariance_matrix_(i,i));
        for (unsigned j = 0; j < covariance_matrix_.size1(); ++j) {
          covariance_matrix_(i,j) = covariance_matrix_(i,j) * multiply_covariance;
          covariance_matrix_(j,i) = covariance_matrix_(j,i) * multiply_covariance;
        }
      } else if(correlation_method_ == PARAM_CORRELATION) {
        covariance_matrix_(i,i) = correlation_diff_ * difference_bounds[i];
      }
    }
  }
}

/**
 * Perform cholesky decomposition on our covariance
 * matrix before it's used in the MCMC.
 *
 * @return true on success, false on failure
 */
bool MCMC::DoCholeskyDecmposition() {
  if (covariance_matrix_.size1() != covariance_matrix_.size2() )
      LOG_ERROR("Invalid covariance matrix (size1!=size2)");

    unsigned matrix_size1 = covariance_matrix_.size1();
    covariance_matrix_lt = covariance_matrix_;

    for (unsigned i = 0; i < matrix_size1; ++i) {
      for (unsigned j = 0; j < matrix_size1; ++j) {
        covariance_matrix_lt(i,j) = 0.0;
      }
    }

    for (unsigned i = 0; i < matrix_size1; ++i) {
      covariance_matrix_lt(i,i) = 1.0;
    }

    if (covariance_matrix_(0,0) < 0)
      return false;

    Double sum = 0.0;

    covariance_matrix_lt(0,0) = sqrt(covariance_matrix_(0,0));

    for (unsigned i = 1; i < matrix_size1; ++i)
      covariance_matrix_lt(i,0) = covariance_matrix_(i,0)/covariance_matrix_lt(0,0);

    for (unsigned i = 1; i < matrix_size1; ++i) {
      sum = 0.0;
      for (unsigned j = 0; j < i; ++j)
        sum += covariance_matrix_lt(i,j) * covariance_matrix_lt(i,j);

      if (covariance_matrix_(i,i) <= sum)
        return false;

      covariance_matrix_lt(i,i) = sqrt(covariance_matrix_(i,i)-sum);
      for (unsigned j = i+1; j < matrix_size1; ++j) {
        sum = 0.0;
        for (unsigned k = 0; k < i; ++k)
          sum += covariance_matrix_lt(j,k) * covariance_matrix_lt(i,k);
        covariance_matrix_lt(j,i) = (covariance_matrix_(j,i)-sum)/covariance_matrix_lt(i,i);
      }
    }

    sum = 0.0;
    for (unsigned i = 0; i < (matrix_size1-1); ++i)
      sum += covariance_matrix_lt(matrix_size1-1,i) * covariance_matrix_lt(matrix_size1-1,i);
    if (covariance_matrix_(matrix_size1-1,matrix_size1-1) <= sum)
      return false;
    covariance_matrix_lt(matrix_size1-1,matrix_size1-1) = sqrt(covariance_matrix_(matrix_size1-1,matrix_size1-1) - sum);

   return true;
}

/**
 * Generate a set of random starting values for our estimates
 */
void MCMC::GenerateRandomStart() {
  vector<Double> original_candidates = candidates_;
  vector<EstimatePtr> estimates = estimates::Manager::Instance().GetEnabled();

  unsigned attempts = 0;
  bool candidates_pass = false;

  if (candidates_.size() != estimate_count_)
    LOG_CODE_ERROR("candidates_.size() != estimate_count_");

  do {
    candidates_pass = true;
    attempts++;
    if (attempts > 1000)
      LOG_ERROR("Failed to generate random start after 1,000 attempts");

    candidates_ = original_candidates;
    FillMultivariateNormal(start_);

    for (unsigned i = 0; i < estimates.size(); ++i) {
      if (estimates[i]->lower_bound() > candidates_[i] || estimates[i]->lower_bound() < candidates_[i]) {
        candidates_pass = false;
        break;
      }
    }

  } while (!candidates_pass);
}

/**
 * Fill the candidates with an attempt using a multivariate normal
 */
void MCMC::FillMultivariateNormal(Double step_size) {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  vector<Double>  normals(estimate_count_, 0.0);
  for (unsigned i = 0; i < estimate_count_; ++i) {
    normals[i] = rng.normal();
  }

  for (unsigned i = 0; i < estimate_count_; ++i) {
    Double row_sum = 0.0;
    for (unsigned j = 0; j < estimate_count_; ++j) {
      row_sum += covariance_matrix_lt(i, j) * normals[j];
    }

    if (is_enabled_estimate_[i])
      candidates_[i] += row_sum * step_size;
  }
}

/**
 * Fill candidates with an attempt using a multivariate
 */
void MCMC::FillMultivariateT(Double step_size) {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  vector<Double>  normals(estimate_count_, 0.0);
  vector<Double>  chisquares(estimate_count_, 0.0);
  for (unsigned i = 0; i < estimate_count_; ++i) {
    normals[i] = rng.normal();
    chisquares[i] = 1 / (rng.chi_square(df_) / df_);
  }

  for (unsigned i = 0; i < estimate_count_; ++i) {
    Double row_sum = 0.0;
    for (unsigned j = 0; j < estimate_count_; ++j) {
      row_sum += covariance_matrix_lt(i, j) * normals[j] * chisquares[j];
    }

    if (is_enabled_estimate_[i])
      candidates_[i] += row_sum * step_size;
  }
}

/**
 * Update our MCMC step size if it's required
 * This is done by
 * 1. Checking if the current iteration is in the adapt_step_size vector
 * 2. Modify the step size
 */
void MCMC::UpdateStepSize() {
  if (jumps_since_adapt_ > 0 && successful_jumps_since_adapt_ > 0) {
    if (std::find(adapt_step_size_.begin(), adapt_step_size_.end(), successful_jumps_) == adapt_step_size_.end())
      return;

    step_size_ *= ((Double)successful_jumps_since_adapt_ / (Double)jumps_since_adapt_) * 4.166667;
    step_size_ = AS_DOUBLE(dc::ZeroFun(step_size_, 1e-10));
    jumps_since_adapt_ = 0;
    successful_jumps_since_adapt_ = 0;
  }
}

/**
 * Generate some new estimate candiddates
 */
void MCMC::GenerateNewCandidates() {
  vector<Double> original_candidates = candidates_;

  unsigned attempts = 0;
  bool candidates_ok = false;

  do {
    candidates_ok = true;
    attempts++;
    if (attempts >= 1000)
      LOG_ERROR("Failed to generate new MCMC candidates after 1,000 attempts. Try a new random seed");

    if (proposal_distribution_ == PARAM_NORMAL)
      FillMultivariateNormal(step_size_);
    else if (proposal_distribution_ == PARAM_T)
      FillMultivariateT(step_size_);

    // Check bounds and regenerate if they're not within bounds
    vector<EstimatePtr> estimates = estimates::Manager::Instance().GetEnabled();
    for (unsigned i = 0; i < estimates.size(); ++i) {
      if (estimates[i]->lower_bound() > candidates_[i] || estimates[i]->upper_bound() < candidates_[i]) {
        candidates_ok = false;
        candidates_ = original_candidates;
      }
    }
  } while (!candidates_ok);
}

} /* namespace isam */





























