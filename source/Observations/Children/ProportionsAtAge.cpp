/**
 * @file ProportionsAtAge.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "ProportionsAtAge.h"

#include <algorithm>

#include "Model/Model.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/To.h"

// Namespaces
namespace isam {
namespace observations {

/**
 * Default constructor
 */
ProportionsAtAge::ProportionsAtAge() {
  parameters_.RegisterAllowed(PARAM_MIN_AGE);
  parameters_.RegisterAllowed(PARAM_MAX_AGE);
  parameters_.RegisterAllowed(PARAM_TOLERANCE);
  parameters_.RegisterAllowed(PARAM_OBS);
  parameters_.RegisterAllowed(PARAM_ERROR_VALUE);
  parameters_.RegisterAllowed(PARAM_DELTA);
  parameters_.RegisterAllowed(PARAM_PROCESS_ERROR);
  parameters_.RegisterAllowed(PARAM_AGEING_ERROR);
}

/**
 * Validate configuration file parameters
 */
void ProportionsAtAge::Validate() {
  Observation::Validate();

  CheckForRequiredParameter(PARAM_MIN_AGE);
  CheckForRequiredParameter(PARAM_MAX_AGE);
  CheckForRequiredParameter(PARAM_OBS);

  min_age_            = parameters_.Get(PARAM_MIN_AGE).GetValue<unsigned>();
  max_age_            = parameters_.Get(PARAM_MAX_AGE).GetValue<unsigned>();
  age_plus_           = parameters_.Get(PARAM_AGE_PLUS).GetValue<bool>(true);
  delta_              = parameters_.Get(PARAM_DELTA).GetValue<double>(DELTA);
  tolerance_          = parameters_.Get(PARAM_TOLERANCE).GetValue<double>(0.001);
  process_error_      = parameters_.Get(PARAM_PROCESS_ERROR).GetValue<double>(0.0);
  ageing_error_label_ = parameters_.Get(PARAM_AGEING_ERROR).GetValue<string>("");
  error_values_       = parameters_.Get(PARAM_ERROR_VALUE).GetValues<double>();
  age_spread_         = (max_age_ - min_age_) + 1;

  /**
   * Do some simple checks
   */
  ModelPtr model = Model::Instance();
  if (min_age_ < model->min_age())
    LOG_ERROR(parameters_.location(PARAM_MIN_AGE) << ": min_age (" << min_age_ << ") is less than the model's min_age (" << model->min_age() << ")");
  if (max_age_ > model->max_age())
    LOG_ERROR(parameters_.location(PARAM_MAX_AGE) << ": max_age (" << max_age_ << ") is greater than the model's max_age (" << model->max_age() << ")");
  if (process_error_ < 0.0)
    LOG_ERROR(parameters_.location(PARAM_PROCESS_ERROR) << ": process_error (" << process_error_ << ") cannot be less than 0.0");
  if (delta_ < 0.0)
    LOG_ERROR(parameters_.location(PARAM_DELTA) << ": delta (" << delta_ << ") cannot be less than 0.0");
  if (error_values_.size() != category_labels_.size())
    LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": number error_values provided (" << error_values_.size() << ") does not match the number of "
        << "category collections provided (" << category_labels_.size() << ")");

  /**
   * Ensure error values are consistent for different likelihood types
   * Note: use of C++ lamba functions
   */
  if (likelihood_type_ == PARAM_LOG_NORMAL) {
    std::for_each(std::begin(error_values_), std::end(error_values_), [&](double n) {
      if (n <= 0.0)
        LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": error_value (" << n << ") cannot be equal to or less than 0.0");
    });

  } else if (likelihood_type_ == PARAM_MULTINOMIAL) {
    std::for_each(std::begin(error_values_), std::end(error_values_), [&](double n) {
      if (n < 0.0)
        LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": error_value (" << n << ") cannot be less than 0.0");
    });

  } else
    LOG_ERROR(parameters_.location(PARAM_LIKELIHOOD) << ": likelihood " << likelihood_type_ << " is not supported by the proportions at age observation. "
        << "Supported types are " << PARAM_LOG_NORMAL << " and " << PARAM_MULTINOMIAL);

  /**
   * Validate the number of obs provided matches age spread * category_labels.
   * This is because we'll have 1 set of obs per category collection provided.
   * categories male+female male = 2 collections
   */
  vector<string> obs;
  obs = parameters_.Get(PARAM_OBS).GetValues<string>();
  unsigned obs_expected = age_spread_ * category_labels_.size();
  if (obs.size() != obs_expected)
    LOG_ERROR(parameters_.location(PARAM_OBS) << ": number of obs provided (" << obs.size() << ") does not match the number expected (" << obs_expected
        << "). Number expected is the age spread (" << age_spread_ << ") * category collections (" << category_labels_.size() << ")");

  /**
   * Populate our proportions matrix.
   *
   * If the proportions for a given observation do not sum to 1.0
   * and is off by more than the tolerance rescale them.
   */
  proportions_.resize(category_labels_.size());
  double value = 0.0;
  double total = 0.0;
  for (unsigned i = 0; i < category_labels_.size(); ++i) {
    total = 0.0;
    for (unsigned j = 0; j < age_spread_; ++j) {
      unsigned obs_index = i * age_spread_ + j;
      if (!utilities::To<double>(obs[obs_index], value))
        LOG_ERROR(parameters_.location(PARAM_OBS) << ": obs value (" << obs[obs_index] << ") at index " << obs_index + 1
            << " in the definition could not be converted to a numeric double");

      total += value;
    }

    if (fabs(1.0 - total) > tolerance_) {
      LOG_WARNING(parameters_.location(PARAM_OBS) << ": obs total (" << total << ") exceeds tolerance for comparison to 1.0 for category collection "
          << category_labels_[i] << ". Auto-scaling proportions to 1.0");

      for (unsigned j = 0; j < proportions_[i].size(); ++j)
        proportions_[i][j] /= total;
    }
  }

}

/**
 * Build any runtime relationships we may have and ensure
 * the labels for other objects are valid.
 */
void ProportionsAtAge::Build() {

}

/**
 *
 */
void ProportionsAtAge::PreExecute() {

}

/**
 *
 */
void ProportionsAtAge::Execute() {

}

} /* namespace observations */
} /* namespace isam */
