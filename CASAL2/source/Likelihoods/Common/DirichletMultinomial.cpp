//============================================================================
// Name        : MultinomialDirichletMultinomial.cpp
// Author      : C.Marsh
// Date        : 11/05/22
// Copyright   : Copyright NIWA Science 2020 - www.niwa.co.nz
// Description :
//============================================================================

// Global headers
#include "DirichletMultinomial.h"

#include <cmath>

// Local headers
#include <cmath>
#include <set>

#include "../../Utilities/Math.h"
#include "../../Utilities/RandomNumberGenerator.h"
#include "../../Model/Model.h"
// Namespaces
namespace niwa {
namespace likelihoods {

using std::set;
namespace math = niwa::utilities::math;

DirichletMultinomial::DirichletMultinomial(shared_ptr<Model> model) : Likelihood(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label for the Dirichlet-multinomial distribution", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type of likelihood", "");
  parameters_.Bind<Double>(PARAM_THETA, &theta_, "Theta parameter (account for overdispersion)", "")->set_lower_bound(0.0);
  //parameters_.Bind<string>(PARAM_OVERDISPERSION_TYPE, &theta_model_, "Is theta linear or saturated", "", PARAM_LINEAR)->set_allowed_values({PARAM_LINEAR,PARAM_SATURATED});
  RegisterAsAddressable(PARAM_THETA, &theta_);
}

/*
* Validate user hasn't supplied a label that is a type from another likelihood.
*/
void DirichletMultinomial::DoValidate() {
  // check labels are not the same as possible type
  if(find(likelihood_types_with_no_labels_.begin(), likelihood_types_with_no_labels_.end(), label_) != likelihood_types_with_no_labels_.end()) 
    LOG_ERROR_P(PARAM_LABEL) << "The label '" << label_ << "' matches a likelihood type. You cannot define a likelihood with a label that is the same as one of the known likelihood types.";
}

/*
* Verify if theta has a transformation
*/
void DirichletMultinomial::DoVerify(shared_ptr<Model> model)  {
  // only verify during an estimation mode or testing
  if((model->run_mode() == RunMode::kEstimation) || (model->run_mode() == RunMode::kProfiling) || (model->run_mode() == RunMode::kMCMC) || (model->run_mode() == RunMode::kTesting)) {
    if (!IsAddressableUsedFor(PARAM_THETA, addressable::kTransformation)) {
      LOG_ERROR_P(PARAM_THETA) << " could not find an @" << PARAM_PARAMETER_TRANSFORMATION << " block for " << PARAM_THETA << ", this is recommeded.";
    }
  }
}
/**
 * Adjust the error value based on the process error
 *
 * @param process_error The observations process_error
 * @param error_value The observations error_value
 * @return An adjusted error value
 */
Double DirichletMultinomial::AdjustErrorValue(const Double process_error, const Double error_value) {
  if ((error_value > 0.0) && (process_error > 0.0))
    return (error_value * process_error);

  return error_value;
}

/*
 * Calculate the scores
 *
 * @param comparisons A collection of comparisons passed by the observation
 * for inspiration https://github.com/timjmiller/wham/blob/28cab35ede66506dcaa73f7de8fc0334870893fc/src/age_comp_osa.hpp
 */
void DirichletMultinomial::GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) {
  Double observed_number;
  Double expected_number;
  for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
    //score += math::LnGamma(N + 1.0) + math::LnGamma(phi) - math::LnGamma(N + phi); // moved to initialscore() calcualtion below;
    for (observations::Comparison& comparison : year_iterator->second) {
      observed_number = comparison.observed_ * comparison.error_value_;
      expected_number = comparison.expected_ * theta_ * comparison.error_value_;
      // calcualte the 'effective' N as adjusted_error_
      comparison.adjusted_error_ = (1.0 + theta_ * comparison.error_value_) / (1.0 + theta_);
      // calculate negative loglikelihood
      comparison.score_ = math::LnGamma(observed_number + 1.0) + math::LnGamma(expected_number) - math::LnGamma(observed_number + expected_number);
    }
  }
}

/**
 * Simulate observed values
 *
 * @param comparisons A collection of comparisons passed by the observation
 */

void DirichletMultinomial::SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) {
  LOG_FATAL() << "Simulation for DirichletMultinomial not implemented yet";

}

/**
 * Calculate the initial score
 *
 * @param comparisons A collection of comparisons passed by the observation
 */

Double DirichletMultinomial::GetInitialScore(map<unsigned, vector<observations::Comparison> >& comparisons, unsigned year) {
  Double score = 0.0;
  Double n = 0.0;
  for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
    n = 0.0;
    for (observations::Comparison& comparison : year_iterator->second) {
      n += comparison.observed_ * comparison.error_value_;
    }
    score += math::LnGamma(n + theta_ * n) - math::LnGamma(n + 1.0) - math::LnGamma(theta_ * n);
  }
  return score;
}

} /* namespace likelihoods */
} /* namespace niwa */
