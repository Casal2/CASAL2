/**
 * @file MCMC.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 10/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */

// headers
#include "MCMC.h"

#include "../EstimateTransformations/Manager.h"
#include "../Estimates/Manager.h"
#include "../GlobalConfiguration/GlobalConfiguration.h"
#include "../Model/Managers.h"
#include "../Model/Model.h"
#include "../ObjectiveFunction/ObjectiveFunction.h"
#include "../Reports/Common/MCMCObjective.h"
#include "../Reports/Common/MCMCSample.h"
#include "../Reports/Manager.h"
#include "../Utilities/Math.h"
#include "../Utilities/RandomNumberGenerator.h"

// namespaces
namespace niwa {

namespace math = niwa::utilities::math;

/**
 * @brief Construct a new MCMC::MCMC object
 *
 * @param model
 */
MCMC::MCMC(shared_ptr<Model> model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label of the MCMC", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The MCMC method", "", PARAM_RANDOMWALK)->set_allowed_values({PARAM_METROPOLIS_HASTINGS, PARAM_HAMILTONIAN, PARAM_RANDOMWALK});
  parameters_.Bind<unsigned>(PARAM_LENGTH, &length_, "The number of iterations for the MCMC (including the burn in period)", "")->set_lower_bound(1);
  parameters_.Bind<unsigned>(PARAM_BURN_IN, &burn_in_, "The number of iterations for the burn_in period of the MCMC", "", 0u)->set_lower_bound(0);
  parameters_.Bind<bool>(PARAM_ACTIVE, &active_, "Indicates if this is the active MCMC algorithm", "", true);
  parameters_.Bind<double>(PARAM_STEP_SIZE, &step_size_, "Initial step-size (as a multiplier of the approximate covariance matrix)", "", 0.02)->set_lower_bound(0);
  parameters_.Bind<double>(PARAM_START, &start_, "The covariance multiplier for the starting point of the MCMC", "", 0.0)->set_lower_bound(0.0);
  parameters_.Bind<unsigned>(PARAM_KEEP, &keep_, "The spacing between recorded values in the MCMC", "", 1u)->set_lower_bound(1u);
  parameters_.Bind<double>(PARAM_MAX_CORRELATION, &max_correlation_, "The maximum absolute correlation in the covariance matrix of the proposal distribution", "", 0.8)
      ->set_range(0.0, 1.0, false, true);
  parameters_
      .Bind<string>(PARAM_COVARIANCE_ADJUSTMENT_METHOD, &correlation_method_, "The method for adjusting small variances in the covariance proposal matrix", "", PARAM_CORRELATION)
      ->set_allowed_values({PARAM_COVARIANCE, PARAM_CORRELATION, PARAM_NONE});
  parameters_
      .Bind<double>(PARAM_CORRELATION_ADJUSTMENT_DIFF, &correlation_diff_,
                    "The minimum non-zero variance times the range of the bounds in the covariance matrix of the proposal distribution", "", 0.0001)
      ->set_lower_bound(0.0, false);
  parameters_.Bind<string>(PARAM_PROPOSAL_DISTRIBUTION, &proposal_distribution_, "The shape of the proposal distribution (either the t or the normal distribution)", "", PARAM_T)
      ->set_allowed_values({PARAM_NORMAL, PARAM_T});
  parameters_.Bind<unsigned>(PARAM_DF, &df_, "The degrees of freedom of the multivariate t proposal distribution", "", 4)->set_lower_bound(0, false);
  parameters_.Bind<unsigned>(PARAM_ADAPT_STEPSIZE_AT, &adapt_step_size_, "The iteration numbers in which to check and resize the MCMC stepsize", "", true)->set_lower_bound(0);
  parameters_.Bind<string>(PARAM_ADAPT_STEPSIZE_METHOD, &adapt_stepsize_method_, "The method to use to adapt the step size", "", PARAM_RATIO)
      ->set_allowed_values({PARAM_RATIO, PARAM_DOUBLE_HALF});
  parameters_.Bind<unsigned>(PARAM_ADAPT_COVARIANCE_AT, &adapt_covariance_matrix_, "The iteration number in which to adapt the covariance matrix", "", 0u)->set_lower_bound(0);
}
#ifdef USE_AUTODIFF
/**
 * @brief During an autodiff build we don't use MCMC,
 * so just disable all the methods with stubs
 *
 */
void MCMC::Validate() {}
void MCMC::Build() {}
void MCMC::Execute(shared_ptr<ThreadPool> thread_pool) {}

void MCMC::ResumeChain() {}
void MCMC::GenerateRandomStart() {}
void MCMC::GenerateNewCandidates() {}
void MCMC::FillMultivariateNormal(double step_size) {}
void MCMC::FillMultivariateT(double step_size) {}
void MCMC::UpdateStepSize() {}
void MCMC::BuildCovarianceMatrix() {}
void MCMC::UpdateCovarianceMatrix() {}
bool MCMC::DoCholeskyDecomposition() {
  return true;
}
bool MCMC::WithinBounds() {
  return true;
}

#else
/**
 * Validate the parameters defined in the configuration file
 */
void MCMC::Validate() {
  parameters_.Populate(model_);

  if (burn_in_ > length_)
    LOG_ERROR_P(PARAM_BURN_IN) << "(" << burn_in_ << ") cannot be greater than the length of the MCMC (" << length_ << ")";

  for (unsigned adapt : adapt_step_size_) {
    if (adapt > burn_in_)
      LOG_ERROR_P(PARAM_ADAPT_STEPSIZE_AT) << "(" << adapt << ") cannot be greater than the length of the burn in(" << burn_in_ << ")";
  }
  if (adapt_covariance_matrix_ > burn_in_)
    LOG_ERROR_P(PARAM_ADAPT_COVARIANCE_AT) << "(" << adapt_covariance_matrix_ << ") cannot be greater than the length of the burn in(" << burn_in_ << ")";

  DoValidate();
}

/**
 * Build any relationships and objects that will hold data
 * so they can be used during the execution
 */
void MCMC::Build() {
  LOG_TRACE();

  estimates_ = model_->managers()->estimate()->GetIsMCMCd();
  if (estimates_.size() == 0)
    LOG_FATAL() << "No estimates are available for use in the MCMC. Either none have been defined, or they have all been set as " << PARAM_MCMC_FIXED;
  if (step_size_ == 0.0)
    step_size_ = 2.4 * pow((double)estimate_count_, -0.5);

  estimate_count_ = estimates_.size();
  candidates_.assign(estimate_count_, 0.0);

  // TODO: Check if Minimiser is being used and if so it has a covariance matrix enabled.

  DoBuild();
}

/**
 * Execute the MCMC
 */
void MCMC::Execute(shared_ptr<ThreadPool> thread_pool) {
  LOG_INFO() << "Starting MCMC " << type_;
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  rng.Reset(model_->global_configuration().random_seed());
  LOG_MEDIUM() << "Resetting RNG with Seed: " << model_->global_configuration().random_seed();

  if (type_ == PARAM_INDEPENDENCE_METROPOLIS)
    return DoExecute(thread_pool);

  LOG_TRACE();
  LOG_MEDIUM() << "mcmc type: " << type_;

  // for (size_t i = 0; auto e : estimates_) candidates_[i++] = e->value(); // C++20
  for (unsigned i = 0; i < estimates_.size(); ++i) candidates_[i] = estimates_[i]->value();

  if (!model_->global_configuration().resume_mcmc()) {
    BuildCovarianceMatrix();
    successful_jumps_ = starting_iteration_;
  } else
    ResumeChain();

  if (!DoCholeskyDecomposition())
    LOG_FATAL() << "Cholesky decomposition failed. Cannot continue MCMC";

  if (start_ > 0.0)
    GenerateRandomStart();

  LOG_MEDIUM() << "First iteration estimates";
  for (unsigned i = 0; i < estimate_count_; ++i) {
    LOG_MEDIUM() << estimates_[i]->label() << " : " << candidates_[i];
    estimates_[i]->set_value(candidates_[i]);
  }

  model_->FullIteration();
  ObjectiveFunction& obj_function = model_->objective_function();
  obj_function.CalculateScore();

  /**
   * Create the first link for the chain. If this is a new run then
   * the values will come from the GenerateRandomStart(), if not then
   * we'll be running the last
   */
  mcmc::ChainLink new_link{.iteration_                   = 1,
                           .mcmc_state_                  = PARAM_INITIALISATION,
                           .score_                       = obj_function.score(),
                           .likelihood_                  = obj_function.likelihoods(),
                           .prior_                       = obj_function.priors(),
                           .penalty_                     = obj_function.penalties(),
                           .additional_priors_           = obj_function.additional_priors(),
                           .jacobians_                   = obj_function.jacobians(),
                           .acceptance_rate_             = 0,
                           .acceptance_rate_since_adapt_ = 0,
                           .step_size_                   = step_size_,
                           .values_                      = candidates_};

  if (!model_->global_configuration().resume_mcmc()) {
    jumps_++;
    jumps_since_adapt_++;
    chain_.push_back(new_link);
  } else {
    LOG_INFO() << "Resuming MCMC chain with iteration " << jumps_;
    mcmc_state_                           = PARAM_RESUME;
    new_link.iteration_                   = jumps_;
    new_link.mcmc_state_                  = mcmc_state_;
    new_link.acceptance_rate_             = acceptance_rate_;
    new_link.acceptance_rate_since_adapt_ = acceptance_rate_since_last_adapt_;
    chain_.push_back(new_link);
  }
  // Make sure we print the first link of the chain
  model_->managers()->report()->Execute(model_, State::kIterationComplete);

  if (length_ == 1)
    return;

  DoExecute(thread_pool);
  LOG_INFO() << "MCMC execution complete";
}

/**
 * @brief Set up the MCMC information for resuming iterations.
 *
 */
void MCMC::ResumeChain() {
  // Since we're resuming, find the last time we adapted that was
  // before the last recorded iteration
  unsigned jumps_since_last_adapt = 1;
  for (unsigned last_adapted : adapt_step_size_) {
    jumps_since_last_adapt = last_adapted < starting_iteration_ ? last_adapted : jumps_since_last_adapt;
  }
  LOG_MEDIUM() << "Chain last adapted at " << jumps_since_last_adapt;

  jumps_             = starting_iteration_;
  jumps_since_adapt_ = jumps_ - jumps_since_last_adapt;

  double temp_success_jumps     = (double)jumps_since_adapt_ * acceptance_rate_since_last_adapt_;
  successful_jumps_since_adapt_ = (unsigned)temp_success_jumps;
  LOG_FINE() << "jumps = " << jumps_ << "jumps since last adapt " << jumps_since_adapt_ << " successful jumps since last adapt " << successful_jumps_since_adapt_ << " step size "
             << step_size_ << " successful jumps " << successful_jumps_;
}

/**
 * @brief Generate a set of candidate values for our
 * MCMC algorithm to start from.
 *
 * This is not used when we're doing a chain resume.
 *
 */
void MCMC::GenerateRandomStart() {
  LOG_INFO() << "Generating random start values for MCMC";
  vector<double>    original_candidates = candidates_;
  vector<Estimate*> estimates           = model_->managers()->estimate()->GetIsEstimated();

  LOG_MEDIUM() << "candidates: ";
  for (auto v : candidates_) LOG_MEDIUM() << v;

  unsigned attempts        = 0;
  bool     candidates_pass = false;

  if (candidates_.size() != estimate_count_)
    LOG_CODE_ERROR() << "candidates_.size() != estimate_count_";

  do {
    candidates_pass = true;
    attempts++;
    if (attempts > 1000)
      LOG_FATAL() << "Failed to generate random start values after 1000 attempts";

    candidates_ = original_candidates;
    FillMultivariateNormal(start_);
    for (unsigned i = 0; i < estimates.size(); ++i) {
      if (estimates[i]->lower_bound() > candidates_[i] || estimates[i]->upper_bound() < candidates_[i]) {
        candidates_pass = false;
        break;
      }
    }

  } while (!candidates_pass);
  LOG_MEDIUM() << "chosen candidates: ";
  for (auto v : candidates_) LOG_MEDIUM() << v;
}

/**
 * @brief Generate new candidate values for the next call through to
 * the model
 *
 */
void MCMC::GenerateNewCandidates() {
  // LOG_MEDIUM() << step_size_;
  if (proposal_distribution_ == PARAM_NORMAL)
    FillMultivariateNormal(step_size_);
  else if (proposal_distribution_ == PARAM_T)
    FillMultivariateT(step_size_);

  LOG_MEDIUM() << "New Candidates";
  for (auto v : candidates_) LOG_MEDIUM() << v;
}

/**
 * @brief Generate new candidates using multivariate normal distribution
 *
 * @param step_size
 */
void MCMC::FillMultivariateNormal(double step_size) {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  vector<double>                    dv(estimate_count_, 0.0);
  vector<double>                    normals(estimate_count_, 0.0);

  for (unsigned i = 0; i < estimate_count_; ++i) normals[i] = rng.normal();

  // Method from CASAL's algorithm
  for (unsigned i = 0; i < estimate_count_; ++i) {
    for (unsigned j = 0; j < estimate_count_; ++j) {
      dv[i] += covariance_matrix_lt(i, j) * normals[j];
    }
    candidates_[i] += dv[i] * step_size;
  }
}

/**
 * Fill the candidates with an attempt using a multivariate t-distribution
 */
void MCMC::FillMultivariateT(double step_size) {
  LOG_MEDIUM() << "FillMultivariateT with step_size: " << step_size;
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  vector<double>                    normals(estimate_count_, 0.0);
  vector<double>                    chisquares(estimate_count_, 0.0);

  for (unsigned i = 0; i < estimate_count_; ++i) {
    double rng_normal = rng.normal();
    normals[i]        = rng_normal;
    LOG_MEDIUM() << "rng_normal: " << rng_normal;
    chisquares[i] = 1 / (rng.chi_squared(df_) / df_);
  }

  for (unsigned i = 0; i < estimate_count_; ++i) {
    double row_sum = 0.0;
    for (unsigned j = 0; j < estimate_count_; ++j) {
      row_sum += covariance_matrix_lt(i, j) * normals[j] * chisquares[j];
    }

    candidates_[i] += row_sum * step_size;
  }
}

/**
 * Get the covariance matrix from the minimiser and then
 * adjust it for the proposal distribution
 */
void MCMC::BuildCovarianceMatrix() {
  LOG_MEDIUM() << "Building covariance matrix";
  // Are we starting at MPD or recalculating the matrix based on an empirical sample
  ublas::matrix<double> original_correlation;
  if (recalculate_covariance_) {
    LOG_MEDIUM() << "Recalculating covariance matrix";
    covariance_matrix_ = covariance_matrix_lt;
  }

  // Remove for the shared library only used for debugging purposes
  // Minimiser* minimiser = model_->managers()->minimiser()->active_minimiser();
  // covariance_matrix_ = minimiser->covariance_matrix();
  // original_correlation = minimiser->correlation_matrix();

  // This is already built by MPD.cpp at line 137. in the frontend the minimiser is dropped out before
  // the MCMC state kicks in, so this will return an incorrect covariance matrix

  if (correlation_method_ == PARAM_NONE)
    return;

  /**
   * Adjust the covariance matrix for the proposal distribution
   */
  LOG_MEDIUM() << "Printing covariance matrix before applying the correlation adjustment";
  for (unsigned i = 0; i < covariance_matrix_.size1(); ++i) {
    for (unsigned j = 0; j < covariance_matrix_.size2(); ++j) {
      LOG_MEDIUM() << "row = " << i + 1 << " col = " << j + 1 << " value = " << covariance_matrix_(i, j);
    }
  }
  ublas::matrix<double> original_covariance(covariance_matrix_);

  LOG_MEDIUM() << "Beginning correlation adjustment. rows = " << original_covariance.size1() << " cols = " << original_covariance.size2();
  if (original_covariance.size1() == 0 || original_covariance.size2() == 0)
    LOG_FATAL() << "No covariance matrix has been provided to the MCMC algorithm";
  // Q: is this section supposed to adjust the full matrix (except for the diagonal) or not? see pg. 67 (79) of User Manual (item #3)
  for (unsigned i = 0; i < (covariance_matrix_.size1() - 1); ++i) {
    for (unsigned j = i + 1; j < covariance_matrix_.size2(); ++j) {
      // This assumes that the lower and upper triangles match
      double value = original_covariance(i, j) / sqrt(original_covariance(i, i) * original_covariance(j, j));
      LOG_MEDIUM() << "row = " << i + 1 << " col = " << j + 1 << " correlation = " << value;
      if (original_covariance(i, j) / sqrt(original_covariance(i, i) * original_covariance(j, j)) > max_correlation_) {
        covariance_matrix_(i, j) = max_correlation_ * sqrt(original_covariance(i, i) * original_covariance(j, j));
        covariance_matrix_(j, i) = covariance_matrix_(i, j);
        LOG_FINE() << "adjusted lower: row = " << i + 1 << " col = " << j + 1;
      }
      if (original_covariance(i, j) / sqrt(original_covariance(i, i) * original_covariance(j, j)) < -max_correlation_) {
        covariance_matrix_(i, j) = -max_correlation_ * sqrt(original_covariance(i, i) * original_covariance(j, j));
        covariance_matrix_(j, i) = covariance_matrix_(i, j);
        LOG_FINE() << "adjusted higher: row = " << i + 1 << " col = " << j + 1;
      }
    }
  }

  LOG_MEDIUM() << "Printing upper triangle of covariance matrix";
  for (unsigned i = 0; i < (covariance_matrix_.size1() - 1); ++i) {
    for (unsigned j = i + 1; j < covariance_matrix_.size2(); ++j) {
      LOG_MEDIUM() << "row = " << i + 1 << " col = " << j + 1 << " value = " << covariance_matrix_(i, j);
    }
  }

  /**
   * Adjust any non-zero variances less than min_diff_ * difference between bounds
   */
  vector<double>    difference_bounds;
  vector<Estimate*> estimates = model_->managers()->estimate()->GetIsEstimated();
  LOG_MEDIUM() << "upper_bound lower_bound";
  for (Estimate* estimate : estimates) {
    difference_bounds.push_back(estimate->upper_bound() - estimate->lower_bound());
    LOG_MEDIUM() << estimate->upper_bound() << " " << estimate->lower_bound();
  }

  for (unsigned i = 0; i < covariance_matrix_.size1(); ++i) {
    if (covariance_matrix_(i, i) < (correlation_diff_ * difference_bounds[i]) && covariance_matrix_(i, i) != 0) {
      if (correlation_method_ == PARAM_COVARIANCE) {
        double multiply_covariance = (sqrt(correlation_diff_) * difference_bounds[i]) / sqrt(covariance_matrix_(i, i));
        LOG_MEDIUM() << "multiplier = " << multiply_covariance << " for parameter = " << i + 1;
        for (unsigned j = 0; j < covariance_matrix_.size2(); ++j) {
          covariance_matrix_(i, j) *= multiply_covariance;
          covariance_matrix_(j, i) *= multiply_covariance;
        }
      } else if (correlation_method_ == PARAM_CORRELATION) {
        covariance_matrix_(i, i) = correlation_diff_ * difference_bounds[i];
      }
    }
  }

  LOG_MEDIUM() << "Printing adjusted covariance matrix";
  for (unsigned i = 0; i < covariance_matrix_.size1(); ++i) {
    for (unsigned j = 0; j < covariance_matrix_.size2(); ++j) {
      LOG_MEDIUM() << "row = " << i + 1 << " col = " << j + 1 << " value = " << covariance_matrix_(i, j);
    }
  }
}

/**
 * Perform Cholesky decomposition on the covariance
 * matrix before it is used in the MCMC.
 *
 * @return true on success, false on failure
 */
bool MCMC::DoCholeskyDecomposition() {
  LOG_TRACE();
  if (covariance_matrix_.size1() != covariance_matrix_.size2())
    LOG_FATAL() << "Invalid covariance matrix (rows != columns). It must be a square matrix";
  unsigned matrix_size1 = covariance_matrix_.size1();
  covariance_matrix_lt  = covariance_matrix_;

  for (unsigned i = 0; i < matrix_size1; ++i) {
    for (unsigned j = 0; j < matrix_size1; ++j) {
      covariance_matrix_lt(i, j) = 0.0;
    }
  }

  for (unsigned i = 0; i < matrix_size1; ++i) covariance_matrix_lt(i, i) = 1.0;

  if (covariance_matrix_(0, 0) < 0)
    return false;

  double sum = 0.0;

  covariance_matrix_lt(0, 0) = sqrt(covariance_matrix_(0, 0));

  for (unsigned i = 1; i < matrix_size1; ++i) covariance_matrix_lt(i, 0) = covariance_matrix_(i, 0) / covariance_matrix_lt(0, 0);

  for (unsigned i = 1; i < matrix_size1; ++i) {
    sum = 0.0;
    for (unsigned j = 0; j < i; ++j) sum += covariance_matrix_lt(i, j) * covariance_matrix_lt(i, j);

    if (covariance_matrix_(i, i) <= sum) {
      LOG_FATAL() << "Cholesky decomposition failed, Singular matrix found, 'covariance_matrix_(i,i) <= sum', for row and column " << i + 1
                  << " parameter = " << estimates_[i]->parameter() << " sum = " << sum << " value = " << covariance_matrix_(i, i);
      return false;
    }
    covariance_matrix_lt(i, i) = sqrt(covariance_matrix_(i, i) - sum);
    for (unsigned j = i + 1; j < matrix_size1; ++j) {
      sum = 0.0;
      for (unsigned k = 0; k < i; ++k) sum += covariance_matrix_lt(j, k) * covariance_matrix_lt(i, k);
      covariance_matrix_lt(j, i) = (covariance_matrix_(j, i) - sum) / covariance_matrix_lt(i, i);
    }
  }

  sum = 0.0;
  for (unsigned i = 0; i < (matrix_size1 - 1); ++i) sum += covariance_matrix_lt(matrix_size1 - 1, i) * covariance_matrix_lt(matrix_size1 - 1, i);
  if (covariance_matrix_(matrix_size1 - 1, matrix_size1 - 1) <= sum) {
    LOG_FATAL() << "Cholesky decomposition failed,  Singular matrix found. 'covariance_matrix_(matrix_size1 - 1, matrix_size1 - 1) <= sum', for row and column " << matrix_size1
                << " sum = " << sum << " value = " << covariance_matrix_(matrix_size1 - 1, matrix_size1 - 1);
    return false;
  }
  covariance_matrix_lt(matrix_size1 - 1, matrix_size1 - 1) = sqrt(covariance_matrix_(matrix_size1 - 1, matrix_size1 - 1) - sum);
  return true;
}

/**
 * Update the MCMC step size if it is required
 * This is done by
 * 1. Checking if the current iteration is in the adapt_step_size vector
 * 2. Modify the step size
 *
 * This is implementing the adapt_at parameter
 */
void MCMC::UpdateStepSize() {
  if (jumps_since_adapt_ > 0 && successful_jumps_since_adapt_ > 0) {
    if (std::find(adapt_step_size_.begin(), adapt_step_size_.end(), jumps_) == adapt_step_size_.end())
      return;
    double old_step_size = step_size_;
    if (adapt_stepsize_method_ == PARAM_RATIO) {
      // modify the stepsize so that AcceptanceRate = 0.24
      step_size_ *= ((double)successful_jumps_since_adapt_ / (double)jumps_since_adapt_) * 4.166667;
      // Ensure the stepsize remains positive
      step_size_ = math::ZeroFun(step_size_, 1e-10);
      // reset counters
    } else if (adapt_stepsize_method_ == PARAM_DOUBLE_HALF) {
      // This is a half or double method really.
      double acceptance_rate;
      if ((double)successful_jumps_since_adapt_ == 0.0)
        acceptance_rate = double(successful_jumps_) / double(jumps_);
      else
        acceptance_rate = (double)successful_jumps_since_adapt_ / (double)jumps_since_adapt_;
      // LOG_MEDIUM() << "acceptance rate since last jump = " << acceptance_rate << " step size " << step_size_ << " numerator = " << ((Double)successful_jumps_ -
      // (Double)successful_jumps_since_adapt_) << " denominator = " << ((Double)jumps_ - (Double)jumps_since_adapt_);
      if (acceptance_rate > 0.5)
        step_size_ *= 2;
      else if (acceptance_rate < 0.2)
        step_size_ /= 2;
      LOG_MEDIUM() << "new step_size = " << step_size_;
    }
    LOG_INFO() << "Adapting step_size from " << old_step_size << " to " << step_size_ << " after " << jumps_ << " iterations";
    return;
  }
}

/**
 * Update the MCMC covariance matrix if it is required
 * This is done by
 * 1. Checking if the current iteration is in the adapt_covariance_matrix vector
 * 2. Modify the covariance matrix
 */
void MCMC::UpdateCovarianceMatrix() {
  if (adapt_covariance_matrix_ < 2 || adapt_covariance_matrix_ != jumps_)
    return;

  if (jumps_since_adapt_ > 1000) {
    recalculate_covariance_ = true;
    LOG_INFO() << "Recalculating the covariance matrix after " << jumps_ << " iterations";
    // modify the covariance matrix this algorithm is taken from CASAL, maybe not the best place to take it from

    // number of parameters
    int n_params = chain_[0].values_.size();

    // number of iterations
    int n_iter = chain_.size() - 1;
    LOG_MEDIUM() << "Number of parameters = " << n_params << ", number of iterations used to recalculate covariance = " << n_iter;

    // temp covariance matrix
    ublas::matrix<double> temp_covariance = covariance_matrix_;

    // Mean parameter vector
    vector<double> mean_var(n_params, 1.0);

    for (int i = 0; i < n_params; ++i) {
      double sx = 0.0;
      for (int k = 0; k < n_iter; ++k) {
        sx += (chain_[k].values_[i]);
      }
      mean_var[i] = sx / n_iter;

      LOG_MEDIUM() << "Total = " << sx << "\n";
      LOG_MEDIUM() << "Mean = " << mean_var[i] << "\n";

      double sxx = 0.0;
      for (int k = 0; k < n_iter; ++k) {
        sxx += pow((chain_[k].values_[i]) - mean_var[i], 2);
      }
      double var            = sxx / (n_iter - 1);
      temp_covariance(i, i) = var;
      for (int j = 0; j < i; j++) {
        double sxy = 0;
        for (int k = 0; k < n_iter; k++) {
          sxy += ((chain_[k].values_[i]) - mean_var[i]) * ((chain_[k].values_[j]) - mean_var[j]);
        }
        double cov            = (sxy / (n_iter - 1));
        temp_covariance(i, j) = cov;
        temp_covariance(j, i) = cov;
      }
    }

    for (int i = 0; i < n_params; ++i) {
      for (int k = 0; k < n_params; ++k) {
        LOG_MEDIUM() << "row =  " << i << " "
                     << " col = " << k << " " << temp_covariance(i, k);
      }
    }

    covariance_matrix_lt = temp_covariance;

    // Adjust covariance based on maximum correlations and apply Cholesky decomposition
    BuildCovarianceMatrix();

    LOG_MEDIUM() << "Applying Cholesky decomposition";
    if (!DoCholeskyDecomposition())
      LOG_FATAL() << "Cholesky decomposition failed. Cannot continue MCMC";

    mcmc_state_ = PARAM_ADAPT_COVARIANCE;
    // continue chain
    return;
  } else {
    LOG_WARNING() << "Recalculation of covariance was not done as the number of jumps since the last adapt_stepsize was not greater than 1000";
  }
}

/*
 * Check that the candidates are within bounds
 */
bool MCMC::WithinBounds() {
  for (unsigned i = 0; i < estimates_.size(); ++i) {
    if (estimates_[i]->lower_bound() > candidates_[i] || estimates_[i]->upper_bound() < candidates_[i]) {
      LOG_MEDIUM() << "Estimate outside of bounds = " << estimates_[i]->parameter() << " value = " << candidates_[i] << " u_b = " << estimates_[i]->upper_bound()
                   << " l_b = " << estimates_[i]->lower_bound();
      return false;
    }
  }

  return true;
}
#endif
} /* namespace niwa */
