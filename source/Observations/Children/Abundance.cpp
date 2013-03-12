/**
 * @file Abundance.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Abundance.h"

#include "Catchabilities/Manager.h"
#include "Utilities/To.h"

// Namespaces
namespace isam {
namespace priors {

namespace utils = isam::utilities;

/**
 * Default constructor
 */
Abundance::Abundance() {
  parameters_.RegisterAllowed(PARAM_CATCHABILITY);
  parameters_.RegisterAllowed(PARAM_OBS);
  parameters_.RegisterAllowed(PARAM_ERROR_VALUE);
  parameters_.RegisterAllowed(PARAM_DELTA);
  parameters_.RegisterAllowed(PARAM_PROCESS_ERROR);
}

/*
 * Destructor
 */
Abundance::~Abundance() {
}

/**
 * Validate configuration file parameters
 */
void Abundance::Validate() {
  Observation::Validate();

  CheckForRequiredParameter(PARAM_CATCHABILITY);
  CheckForRequiredParameter(PARAM_OBS);
  CheckForRequiredParameter(PARAM_ERROR_VALUE);

  catchability_label_ = parameters_.Get(PARAM_CATCHABILITY).GetValue<string>();
  delta_              = parameters_.Get(PARAM_DELTA).GetValue<double>(1e-10);
  process_error_      = parameters_.Get(PARAM_PROCESS_ERROR).GetValue<double>(0.0);

  if (delta_ < 0.0)
    LOG_ERROR(parameters_.location(PARAM_DELTA) << ": delta (" << delta_ << ") cannot be less than 0.0");
  if (process_error_ < 0.0)
    LOG_ERROR(parameters_.location(PARAM_PROCESS_ERROR) << ": process_error (" << process_error_ << ") cannot be less than 0.0");

  vector<string> obs  = parameters_.Get(PARAM_OBS).GetValues<string>();
  if (obs.size() % 2 != 0)
    LOG_ERROR(parameters_.location(PARAM_OBS) << ": obs values must be in pairs. e.g obs <category> <value> <category> <value>");

  double value = 0.0;
  for (unsigned i = 0; i < obs.size(); i+=2) {
    if (!utils::To<double>(obs[i + 1], value))
      LOG_ERROR(parameters_.location(PARAM_OBS) << ": obs value " << obs[i + 1] << " cannot be converted to a double");
    if (value <= 0.0)
      LOG_ERROR(parameters_.location(PARAM_OBS) << ": obs value " << value << " cannot be less than or equal to 0.0");

    proportions_[obs[i]] = value;
  }

  vector<string> error_values = parameters_.Get(PARAM_ERROR_VALUE).GetValues<string>();
  if (error_values.size() % 2 != 0)
    LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": error_value values must be in pairs. e.g error_value <category> <value>");

  for (unsigned i = 0; i < error_values.size(); i+=2) {
    if (!utils::To<double>(error_values[i + 1], value))
      LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": error_value " << error_values[i + 1] << " cannot be converted to a double");
    if (value <= 0.0)
      LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": error_value " << value << " cannot be less than or equal to 0.0");

    error_values_[error_values[i]] = value;
  }

  /**
   * Ensure we have an error value for every obs
   */
  map<string, Double>::iterator iter;
  for (iter = proportions_.begin(); iter != proportions_.end(); ++iter) {
    if (error_values_.find(iter->first) == error_values_.end()) {
      LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": No error_value has been specified for the obs category " << iter->first);
    }
  }
}

/**
 * Build any runtime relationships we may have and ensure
 * the labels for other objects are valid.
 */
void Abundance::Build() {
  Observation::Build();

  catchability_ = catchabilities::Manager::Instance().GetCatchability(catchability_label_);
  if (!catchability_)
    LOG_ERROR(parameters_.location(PARAM_CATCHABILITY) << ": catchability " << catchability_label_ << " could not be found. Have you defined it?");
}

/**
 * Run our observation to generate the score
 */
void Abundance::Execute() {
  Observation::Execute();

                  score_              = 0.0;
  double          expected_total      = 0.0;
  vector<string>  keys;
  vector<Double>  expected;
  vector<Double>  process_error;
  vector<Double>  error_value;
  vector<Double>  scores_;

  // Get the partition cache

















}

} /* namespace priors */
} /* namespace isam */
