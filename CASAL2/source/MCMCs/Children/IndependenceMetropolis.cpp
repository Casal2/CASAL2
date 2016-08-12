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
  parameters_.Bind<Double>(PARAM_START, &start_, "Covariance multiplier for the starting point of the MCMC", "", 0.0);
  parameters_.Bind<unsigned>(PARAM_KEEP, &keep_, "Spacing between recorded values in the MCMC", "", 1u);
  parameters_.Bind<Double>(PARAM_MAX_CORRELATION, &max_correlation_, "Maximum absolute correlation in the covariance matrix of the proposal distribution", "", 0.8);
  parameters_.Bind<string>(PARAM_COVARIANCE_ADJUSTMENT_METHOD, &correlation_method_, "Method for adjusting small variances in the covariance proposal matrix"
      , "", PARAM_COVARIANCE);
  parameters_.Bind<Double>(PARAM_CORRELATION_ADJUSTMENT_DIFF, &correlation_diff_, "Minimum non-zero variance times the range of the bounds in the covariance matrix of the proposal distribution", "", 0.0001);
  parameters_.Bind<Double>(PARAM_STEP_SIZE, &step_size_, "Initial stepsize (as a multiplier of the approximate covariance matrix)", "", 0.02);
  parameters_.Bind<string>(PARAM_PROPOSAL_DISTRIBUTION, &proposal_distribution_, "The shape of the proposal distribution (either the t or the normal distribution)", "", PARAM_T);
  parameters_.Bind<unsigned>(PARAM_DF, &df_, "Degrees of freedom of the multivariate t proposal distribution", "", 4);
  parameters_.Bind<unsigned>(PARAM_ADAPT_STEPSIZE_AT, &adapt_step_size_, "Iterations in the chain to check and resize the MCMC stepsize", "", true);
  parameters_.Bind<unsigned>(PARAM_ADAPT_COVARIANCE_AT, &adapt_covariance_matrix_, "Iterations in the chain to check and resize the MCMC stepsize", "", true);
  parameters_.Bind<string>(PARAM_ADAPT_STEPSIZE_METHOD, &adapt_stepsize_method_, "Method to adapt step size.", "", PARAM_SPM)->set_allowed_values({PARAM_CASAL, PARAM_SPM});

  jumps_                          = 0;
  successful_jumps_               = 0;
  jumps_since_adapt_              = 0;
  successful_jumps_since_adapt_   = 0;
  step_size_                      = 0.0;
  last_item_                      = false;
}
/**
 * Get the covariance matrix from the minimiser and then
 * adjust it for our proposal distribution
 */
void IndependenceMetropolis::BuildCovarianceMatrix() {
  // Are we starting at MPD or recalculating the matrix based on an empirical sample
  if (recalculate_covariance_)
    covariance_matrix_ = covariance_matrix_lt;
  else
    covariance_matrix_ = minimiser_->covariance_matrix();

  if (correlation_method_ == PARAM_NONE)
    return;

  /**
   * Adjust the covariance matrix for the proposal distribution
   */
  for (unsigned i = 0; i < covariance_matrix_.size1(); ++i) {
    for (unsigned j = i + 1; j < covariance_matrix_.size2(); ++j) {
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
  vector<Estimate*> estimates = model_->managers().estimate()->GetIsEstimated();
  for (Estimate* estimate : estimates) {
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
bool IndependenceMetropolis::DoCholeskyDecmposition() {
  if (covariance_matrix_.size1() != covariance_matrix_.size2() )
      LOG_ERROR() << "Invalid covariance matrix (size1!=size2)";
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
void IndependenceMetropolis::GenerateRandomStart() {
  vector<Double> original_candidates = candidates_;
  vector<Estimate*> estimates = model_->managers().estimate()->GetIsEstimated();

  unsigned attempts = 0;
  bool candidates_pass = false;

  if (candidates_.size() != estimate_count_)
    LOG_CODE_ERROR() << "candidates_.size() != estimate_count_";

  do {
    candidates_pass = true;
    attempts++;
    if (attempts > 1000)
      LOG_ERROR() << "Failed to generate random start after 1,000 attempts";

    candidates_ = original_candidates;
    FillMultivariateNormal(start_);

    for (unsigned i = 0; i < estimates.size(); ++i) {
      if (estimates[i]->lower_bound() > candidates_[i] || estimates[i]->upper_bound() < candidates_[i]) {
    	  candidates_pass = false;
        break;
      }
    }

  } while (!candidates_pass);
}

/**
 * Fill the candidates with an attempt using a multivariate normal
 */
void IndependenceMetropolis::FillMultivariateNormal(Double step_size) {
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
void IndependenceMetropolis::FillMultivariateT(Double step_size) {
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
void IndependenceMetropolis::UpdateStepSize() {
  if (jumps_since_adapt_ > 0 && successful_jumps_since_adapt_ > 0) {
    if (std::find(adapt_step_size_.begin(), adapt_step_size_.end(), jumps_) == adapt_step_size_.end())
      return;
    if (adapt_stepsize_method_ == PARAM_SPM) {
      // modify the stepsize so that AcceptanceRate = 0.24
      step_size_ *= ((Double)successful_jumps_since_adapt_ / (Double)jumps_since_adapt_) * 4.166667;
      // Ensure the stepsize remains positive
      step_size_ = AS_DOUBLE(dc::ZeroFun(step_size_, 1e-10));
      // reset counters
    } else if (adapt_stepsize_method_ == PARAM_CASAL) {
      // This is a half or double method really.
      Double acceptance_rate = ((Double)successful_jumps_ - (Double)successful_jumps_since_adapt_) / ((Double)jumps_ - (Double)jumps_since_adapt_);
      if (acceptance_rate > 0.5)
        step_size_ *= 2;
      else if (acceptance_rate < 0.2)
        step_size_ /= 2;
    }

    jumps_since_adapt_ = 0;
    successful_jumps_since_adapt_ = 0;

    return;
  }
}


/**
 * Update our MCMC Covariance matrix if it's required
 * This is done by
 * 1. Checking if the current iteration is in the adapt_covariance_matrix vector
 * 2. Modify the covariance matrix
 */
void IndependenceMetropolis::UpdateCovarianceMatrix() {
  if (jumps_since_adapt_ > 1000) {
    if (std::find(adapt_covariance_matrix_.begin(), adapt_covariance_matrix_.end(), jumps_) == adapt_covariance_matrix_.end())
      return;
    recalculate_covariance_ = true;
    LOG_MEDIUM() << "Re calculating covariance matrix, after " << chain_.size() << " iterations";
    // modify the covaraince matrix this algorithm is stolen from CASAL, maybe not the best place to take it from
    //number of parameters
    int n_params = chain_[0].values_.size();
    // number of iterations
    int n_iter = chain_.size() - 1;
    LOG_MEDIUM() << "Number of parameters = " << n_params << ", number of iterations used to recalculate covariance = " << n_iter;
    // temp covariance matrix
    ublas::matrix<Double> temp_covariance = covariance_matrix_;
    // Mean parameter vector
    vector<Double> mean_var(n_params, 1.0);
    for (int i = 0; i < n_params; ++i) {
      Double sx = 0.0;
      for (int k = 0; k < n_iter; ++k) {
       sx +=  chain_[k].values_[i];
      }
      mean_var[i] = sx / n_iter;

      LOG_MEDIUM() << "Total = " << sx << "\n";
      LOG_MEDIUM() << "Mean = " << mean_var[i]  << "\n";

      Double sxx = 0.0;
      for (int k = 0; k < n_iter; ++k) {
       sxx += pow(chain_[k].values_[i] - mean_var[i],2);
      }
      Double var = sxx / (n_iter - 1);
      temp_covariance(i,i) = var;
      for (int j = 0; j < i; j++){
        Double sxy = 0;
        for (int k = 0; k < n_iter; k++){
          sxy += (chain_[k].values_[i] - mean_var[i]) * (chain_[k].values_[j] - mean_var[j]);
        }
        Double cov = (sxy / (n_iter - 1));
        temp_covariance(i,j) = cov;
        temp_covariance(j,i) = cov;
      }
    }
    for (int i = 0; i < n_params; ++i){
      for (int k = 0; k < n_params; ++k){
        LOG_MEDIUM() << "row =  " << i << " " << " col = " << k << " " << temp_covariance(i,k);
      }

    }
    covariance_matrix_lt = temp_covariance;

    // Adjust covariance based on maximum correlations and do teh choleskyDecompositon
    BuildCovarianceMatrix();
    if (!DoCholeskyDecmposition())
      LOG_FATAL() << "Cholesky decomposition failed. Cannot continue MCMC";

    // continue chain
    return;
  }
}

/**
 * Generate some new estimate candiddates
 */
void IndependenceMetropolis::GenerateNewCandidates() {
  vector<Double> original_candidates = candidates_;

  unsigned attempts = 0;
  bool candidates_ok = false;

  do {
    candidates_ok = true;
    attempts++;
    if (attempts >= 1000)
      LOG_ERROR() << "Failed to generate new MCMC candidates after 1,000 attempts. Try a new random seed";

    LOG_MEDIUM() << step_size_;
    if (proposal_distribution_ == PARAM_NORMAL)
      FillMultivariateNormal(step_size_);
    else if (proposal_distribution_ == PARAM_T)
      FillMultivariateT(step_size_);

    // Check bounds and regenerate if they're not within bounds
    vector<Estimate*> estimates = model_->managers().estimate()->GetIsEstimated();

    for (unsigned i = 0; i < estimates.size(); ++i) {
      if (estimates[i]->lower_bound() > candidates_[i] || estimates[i]->upper_bound() < candidates_[i]) {
        candidates_ok = false;
        candidates_ = original_candidates;
      }
    }
  } while (!candidates_ok);
}

void IndependenceMetropolis::DoValidate() {
  if (adapt_step_size_.size() == 0)
    adapt_step_size_.assign(1, 1u);

  if (adapt_covariance_matrix_.size() == 0)
    adapt_covariance_matrix_.assign(1, 1u);

  if (adapt_covariance_matrix_.size() > 1)
    LOG_ERROR_P(PARAM_ADAPT_COVARIANCE_AT) << "Currently you can only adapt the covariance matrix once, for a chain";

  if (length_ <= 0)
    LOG_ERROR_P(PARAM_LENGTH) << "(" << length_ << ") cannot be less than or equal to 0";

  for (unsigned adapt : adapt_step_size_) {
    if (adapt < 1)
      LOG_ERROR_P(PARAM_ADAPT_STEPSIZE_AT) << "(" << adapt << ") cannot be less than 1";
    if (adapt > length_)
      LOG_ERROR_P(PARAM_ADAPT_STEPSIZE_AT) << "(" << adapt << ") cannot be greater than length(" << length_ << ")";
  }

  if (correlation_method_ != PARAM_CORRELATION && correlation_method_ != PARAM_COVARIANCE && correlation_method_ != PARAM_NONE)
    LOG_ERROR_P(PARAM_COVARIANCE_ADJUSTMENT_METHOD) << "(" << correlation_method_ << ")"
        << " is not supported. Currently supported values are " << PARAM_CORRELATION << ", " << PARAM_COVARIANCE << " and " << PARAM_NONE;

  if (proposal_distribution_ != PARAM_T && proposal_distribution_ != PARAM_NORMAL)
    LOG_ERROR_P(PARAM_PROPOSAL_DISTRIBUTION) << "(" << proposal_distribution_ << ")"
        << " is not supported. Currently supported values are " << PARAM_T << " and " << PARAM_NORMAL;

  if (max_correlation_ <= 0.0 || max_correlation_ > 1.0)
    LOG_ERROR_P(PARAM_MAX_CORRELATION) << "(" << max_correlation_ << ") must be between 0.0 (not inclusive) and 1.0 (inclusive)";
  if (df_ <= 0)
    LOG_ERROR_P(PARAM_DF) << "(" << df_ << ") cannot be less or equal to 0";
  if (start_ < 0.0)
    LOG_ERROR_P(PARAM_START) << "(" << start_ << ") cannot be less than 0";
  if (step_size_ < 0.0)
    LOG_ERROR_P(PARAM_STEP_SIZE) << "(" << step_size_ << ") cannot be less than 0.0";

}

/**
 *
 */
void IndependenceMetropolis::DoBuild() {
  LOG_TRACE();

  minimiser_ = model_->managers().minimiser()->active_minimiser();

  unsigned active_estimates = 0;

  vector<Estimate*> estimates = model_->managers().estimate()->GetIsEstimated();
  estimate_count_ = estimates.size();
  for (Estimate* estimate : estimates) {
    estimate_labels_.push_back(estimate->label());

    if (estimate->upper_bound() == estimate->lower_bound() || estimate->mcmc_fixed())
      continue;
    active_estimates++;
  }

  if (active_estimates == 0)
    LOG_ERROR() << "While building the MCMC system the number of active estimates was 0. You need at least 1 non-fixed MCMC estimate";

  if (step_size_ == 0.0)
    step_size_ = 2.4 * pow((Double)active_estimates, -0.5);
}

/**
 * Execute the MCMC system and build our MCMC chain
 */
void IndependenceMetropolis::DoExecute() {
  candidates_.resize(estimate_count_);
  is_enabled_estimate_.resize(estimate_count_);

  vector<Estimate*> estimates = model_->managers().estimate()->GetIsEstimated();
  for (unsigned i = 0; i < estimate_count_; ++i) {
    candidates_[i] = AS_DOUBLE(estimates[i]->value());

    if (estimates[i]->lower_bound() == estimates[i]->upper_bound() || estimates[i]->mcmc_fixed())
      is_enabled_estimate_[i] = false;
    else
      is_enabled_estimate_[i] = true;
  }

  if (!model_->global_configuration().resume()) {
    BuildCovarianceMatrix();
  }

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
  Double penalty = AS_DOUBLE(obj_function.penalties());
  Double prior = AS_DOUBLE(obj_function.priors());
  Double likelihood = AS_DOUBLE(obj_function.likelihoods());
  Double additional_prior = AS_DOUBLE(obj_function.additional_priors());
  Double jacobian = AS_DOUBLE(obj_function.jacobians());

  /**
   * Store first location
   */
  {
    mcmc::ChainLink new_link;
    new_link.iteration_                     = starting_iteration_;
    new_link.penalty_                       = AS_DOUBLE(obj_function.penalties());
    new_link.score_                         = AS_DOUBLE(obj_function.score());
    new_link.prior_                         = AS_DOUBLE(obj_function.priors());
    new_link.likelihood_                    = AS_DOUBLE(obj_function.likelihoods());
    new_link.additional_priors_             = AS_DOUBLE(obj_function.additional_priors());
    new_link.jacobians_                     = AS_DOUBLE(obj_function.jacobians());
    new_link.acceptance_rate_               = 0;
    new_link.acceptance_rate_since_adapt_   = 0;
    new_link.step_size_                     = step_size_;
    new_link.values_                        = candidates_;
    chain_.push_back(new_link);
  }

  successful_jumps_ = starting_iteration_;
  /**
   * Now we start the MCMC process
   */
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  do {
    LOG_FINE() << "MCMC Starting";
    LOG_FINE() << "Covariance matrix is: " << covariance_matrix_.size1() << "x" << covariance_matrix_.size2();
    LOG_FINE() << "Estimate Count: " << estimate_count_;

    vector<Double> original_candidates = candidates_;
    UpdateStepSize();
    UpdateCovarianceMatrix();
    GenerateNewCandidates();
    for (unsigned i = 0; i < candidates_.size(); ++i)
      estimates[i]->set_value(candidates_[i]);

    model_->FullIteration();
    obj_function.CalculateScore();

    Double previous_score = score;
    Double previous_prior = prior;
    Double previous_likelihood = likelihood;
    Double previous_penalty = penalty;
    Double previous_additional_prior = additional_prior;
    Double previous_jacobian = jacobian;


    score = AS_DOUBLE(obj_function.score());
    penalty = AS_DOUBLE(obj_function.penalties());
    prior = AS_DOUBLE(obj_function.priors());
    likelihood = AS_DOUBLE(obj_function.likelihoods());
    additional_prior = AS_DOUBLE(obj_function.additional_priors());
    jacobian = AS_DOUBLE(obj_function.jacobians());

    Double ratio = 1.0;

    if (score > previous_score) {
      ratio = exp(-score + previous_score);
      LOG_MEDIUM() << " Current Objective score " << score << " Previous score " << previous_score;
    }

    jumps_++;
    jumps_since_adapt_++;

    if (dc::IsEqual(ratio, 1.0) || rng.uniform() < ratio) {
      LOG_MEDIUM() << "Ratio = " << ratio << " random_number = " << rng.uniform();
      // Accept this jump
      successful_jumps_++;
      successful_jumps_since_adapt_++;
      // Record the score, and its compontent parts if the successful jump divided by keep has no remainder
      // i.e the accepted candidate is a keep value
      if (jumps_ % keep_ == 0) {
        LOG_FINE() << "Keeping jump " << jumps_;
        mcmc::ChainLink new_link;
        new_link.iteration_ = jumps_;
        new_link.penalty_ = obj_function.penalties();
        new_link.score_ = AS_DOUBLE(obj_function.score());
        new_link.prior_ = obj_function.priors();
        new_link.likelihood_ = obj_function.likelihoods();
        new_link.additional_priors_ = obj_function.additional_priors();
        new_link.jacobians_ = obj_function.jacobians();
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
      // Reject this attempt but still record the chain if it lands on a keep
      score = previous_score;
      prior = previous_prior;
      penalty = previous_penalty;
      likelihood = previous_likelihood;
      additional_prior = previous_additional_prior;
      jacobian = previous_jacobian;
      candidates_ = original_candidates;

      if (jumps_ % keep_ == 0) {
        LOG_FINE() << "Keeping jump " << jumps_;
        mcmc::ChainLink new_link;
        new_link.iteration_ = jumps_;
        new_link.penalty_ = previous_penalty;
        new_link.score_ = previous_score;
        new_link.prior_ = previous_prior;
        new_link.likelihood_ = previous_likelihood;
        new_link.additional_priors_ = previous_additional_prior;
        new_link.jacobians_ = previous_jacobian;
        new_link.acceptance_rate_ = Double(successful_jumps_) / Double(jumps_);
        new_link.acceptance_rate_since_adapt_ = Double(successful_jumps_since_adapt_) / Double(jumps_since_adapt_);
        new_link.step_size_ = step_size_;
        new_link.values_ = original_candidates;
        chain_.push_back(new_link);
        LOG_MEDIUM() << "Successful Jumps " << successful_jumps_ << " Jumps : " << jumps_ << " successful jumps since adapt " << successful_jumps_since_adapt_
            << " jumps since adapt " << jumps_since_adapt_;
        model_->managers().report()->Execute(State::kIterationComplete);
      }
    }

    LOG_FINEST() << jumps_ << " jumps have been completed";
  } while (jumps_ <= length_);
}

} /* namespace mcmcs */
} /* namespace niwa */
