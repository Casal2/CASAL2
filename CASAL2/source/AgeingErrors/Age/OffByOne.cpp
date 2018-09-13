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
OffByOne::OffByOne(Model* model) : AgeingError(model) {
  parameters_.Bind<Double>(PARAM_P1, &p1_, "proportion misclassified as one year younger, e.g., the proportion of age 3 individuals that were misclassified as age 2", "")->set_range(0.0, 1.0);
  parameters_.Bind<Double>(PARAM_P2, &p2_, "proportion misclassified as one year older, e.g., the proportion of age 3 individuals that were misclassified as age 4", "")->set_range(0.0, 1.0);
  parameters_.Bind<unsigned>(PARAM_K, &k_, "The minimum age of fish which can be misclassified, i.e., fish of age less than k are assumed to be correctly classified", "", 0u)->set_range(0.0, 1.0);
  RegisterAsAddressable(PARAM_P1, &p1_);
  RegisterAsAddressable(PARAM_P2, &p2_);
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloards
 *
 * Note: all parameters are populated from configuration files
 */
void OffByOne::DoValidate() {
  if (k_ > max_age_)
    LOG_ERROR_P(PARAM_K)<< "value (" << k_ << ") cannot be greater than the model's max age (" << max_age_ << ")";
}

void OffByOne::DoBuild() {
  DoReset();
}

/**
 * Reset our mis_matrix to ensure it has the latest
 * changes from any addressable modifications
 */
void OffByOne::DoReset() {
  mis_matrix_[0][0] = 1.0 - p2_;
  mis_matrix_[0][1] = p2_;
  for (unsigned i = 1; i < (mis_matrix_.size() - 1); ++i) {
    LOG_FINEST() << " did we enter the loop?";
    mis_matrix_[i][i - 1] = p1_;
    mis_matrix_[i][i] = 1.0 - (p1_ + p2_);
    mis_matrix_[i][i + 1] = p2_;
  }

  mis_matrix_[age_spread_ - 1][age_spread_ - 2] = p1_;

  if (plus_group_) {
    mis_matrix_[age_spread_ - 1][age_spread_ - 1] = 1.0 - p1_;
  } else {
    mis_matrix_[age_spread_ - 1][age_spread_ - 1] = 1.0 - (p1_ + p2_);
  }

  if (k_ > min_age_) {
    unsigned l = 0;
    for (unsigned j = 0; j < (k_ - min_age_); ++j, ++l) {
      mis_matrix_[j][l] = 0.0;
      mis_matrix_[j][j] = 1.0;
    }
  }

}


} /* namespace ageingerrors */
} /* namespace niwa */
