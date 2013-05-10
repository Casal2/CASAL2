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

// namespaces
namespace isam {

/**
 * Constructor
 */
MCMC::MCMC() {
  parameters_.RegisterAllowed(PARAM_START);
  parameters_.RegisterAllowed(PARAM_LENGTH);
  parameters_.RegisterAllowed(PARAM_KEEP);
  parameters_.RegisterAllowed(PARAM_LENGTH);
  parameters_.RegisterAllowed(PARAM_MAX_CORRELATION);
  parameters_.RegisterAllowed(PARAM_COVARIANCE_ADJUSTMENT_METHOD);
  parameters_.RegisterAllowed(PARAM_CORRELATION_ADJUSTMENT_DIFF);
  parameters_.RegisterAllowed(PARAM_STEP_SIZE);
  parameters_.RegisterAllowed(PARAM_PROPOSAL_DISTRIBUTION);
  parameters_.RegisterAllowed(PARAM_DF);
  parameters_.RegisterAllowed(PARAM_ADAPT_STEPSIZE_AT);

  jumps_                          = 0;
  successful_jumps_               = 0;
  jumps_since_adapt_              = 0;
  successful_jumps_since_adapt_   = 0;
  step_size_                      = 0.0;
  last_item_                      = false;
}

/**
 * Validate the parameters defined in the configuration file
 */
void MCMC::Validate() {
  CheckForRequiredParameter(PARAM_LENGTH);

  start_                  = parameters_.Get(PARAM_START).GetValue<double>(0.0);
  length_                 = parameters_.Get(PARAM_LENGTH).GetValue<unsigned>();
  keep_                   = parameters_.Get(PARAM_KEEP).GetValue<unsigned>(1u);
  max_correlation_        = parameters_.Get(PARAM_MAX_CORRELATION).GetValue<double>(0.8);
  correlation_method_     = parameters_.Get(PARAM_COVARIANCE_ADJUSTMENT_METHOD).GetValue<string>(PARAM_COVARIANCE);
  correlation_diff_       = parameters_.Get(PARAM_CORRELATION_ADJUSTMENT_DIFF).GetValue<double>(0.0001);
  step_size_              = parameters_.Get(PARAM_STEP_SIZE).GetValue<double>(0.0);
  proposal_distribution_  = parameters_.Get(PARAM_PROPOSAL_DISTRIBUTION).GetValue<string>(PARAM_T);
  df_                     = parameters_.Get(PARAM_DF).GetValue<unsigned>(4);

  if (parameters_.IsDefined(PARAM_ADAPT_STEPSIZE_AT))
    adapt_step_size_        = parameters_.Get(PARAM_ADAPT_STEPSIZE_AT).GetValues<unsigned>();
  else
    adapt_step_size_.assign(1, 1u);

  if (length_ <= 0)
    LOG_ERROR(parameters_.location(PARAM_LENGTH) << "(" << length_ << ") cannot be less than or equal to 0");

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

}

/**
 *
 */
void MCMC::Execute() {

}

} /* namespace isam */
