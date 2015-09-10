/**
 * @file OffByOne.cpp
 * @author C Marsh
 * @github https://github.com/Zaita
 * @date 01/09/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "OffByOne.h"

// namespaces
namespace niwa {
namespace ageingerrors {

// Constructor
OffByOne::OffByOne() {
  parameters_.Bind<Double>(PARAM_P1, &p1_, "proprtion of misclassification up by an age, i.e. how proportion of age 3 that are actually age 4", "");
  parameters_.Bind<Double>(PARAM_P2, &p2_, "proprtion of misclassification down by an age", "");
  parameters_.Bind<unsigned>(PARAM_K, &k_, "The minimum age of fish whcih can be missclassified", "", 0u);

  RegisterAsEstimable(PARAM_P1, &p1_);
  RegisterAsEstimable(PARAM_P2, &p2_);
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloards
 *
 * Note: all parameters are populated from configuration files
 */
void OffByOne::DoValidate() {
  if (k_ > max_age_)
    LOG_ERROR_P(PARAM_K) << "value (" << k_ << ") cannot be greater than the model's max age (" << max_age_ << ")";
  if (p1_ < 0.0)
    LOG_ERROR_P(PARAM_P1) << PARAM_P1 << " Cannot be less than 0.0";
  if (p2_ > 1.0)
    LOG_ERROR_P(PARAM_P2) << PARAM_P2 << " Cannot be greater than 1.0";

}

void OffByOne::DoBuild() {
  DoReset();
}

void OffByOne::DoReset() {

  mis_matrix_[0][0] = 1.0 - p2_;
  mis_matrix_[0][1] = p2_;
  for (unsigned i = 0; i < age_spread_; ++i) {
  if (k_ > min_age_) {
      mis_matrix_[i][i - 1.0] = p1_;
      mis_matrix_[i][i] = 1.0 - p1_ - p2_;
      mis_matrix_[i][i + 1.0] = p1_;
    }
  mis_matrix_[age_spread_][age_spread_ - 1.0] = p1_;
  }

  if (age_plus_) {
    mis_matrix_[age_spread_][age_spread_] = 1.0 - p1_;
  } else {
    mis_matrix_[age_spread_][age_spread_] = 1.0 - p1_ - p2_;
  }

  for (unsigned j = 0; j < k_; ++j) {
    mis_matrix_[j].assign(mis_matrix_[j].size(), 0.0);
    mis_matrix_[j][j] = 1.0;
  }

}


} /* namespace ageingerrors */
} /* namespace niwa */
