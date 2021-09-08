/**
 * @file Multinomial.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 25/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Multinomial.h"

#include <cmath>
#include <set>

#include "../../Utilities/Math.h"
#include "../../Utilities/RandomNumberGenerator.h"

// Namespaces
namespace niwa {
namespace likelihoods {

using std::set;
namespace math = niwa::utilities::math;

/**
 * Adjust the error value based on the process error
 *
 * @param process_error The observations process_error
 * @param error_value The observations error_value
 * @return An adjusted error value
 */
Double Multinomial::AdjustErrorValue(const Double process_error, const Double error_value) {
  if (process_error > 0.0 && error_value > 0.0)
    return (1.0 / (1.0 / error_value + 1.0 / process_error));

  return error_value;
}

/**
 * Calculate the scores
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void Multinomial::GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) {
  LOG_TRACE();
  for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
    for (observations::Comparison& comparison : year_iterator->second) {
      Double error_value = AdjustErrorValue(comparison.process_error_, comparison.error_value_) * error_value_multiplier_;

      Double score = math::LnFactorial(error_value * comparison.observed_) - error_value * comparison.observed_ * log(math::ZeroFun(comparison.expected_, comparison.delta_));

      comparison.adjusted_error_ = error_value;
      comparison.score_          = score * multiplier_;

      // if (isnan(AS_DOUBLE(comparison.score_))) {
      //   LOG_CODE_ERROR() << "One of the comparison scores came back as NaN... memory bug somewhere";
      // }
    }
  }
  LOG_TRACE();
}

/**
 * Calculate the initial score
 *
 * @param comparisons A collection of comparisons passed by the observation
 */

Double Multinomial::GetInitialScore(map<unsigned, vector<observations::Comparison> >& comparisons, unsigned year) {
  Double score = 0.0;

  // int stopper = 0;
  observations::Comparison& comparison = comparisons[year][0];
  // if (stopper == 1)
  //  break;
  Double temp_score = -math::LnFactorial(AdjustErrorValue(comparison.process_error_, comparison.error_value_) * error_value_multiplier_);
  LOG_FINEST() << "Adding: " << temp_score << " = LnFactorial(AdjustErrorValue(" << comparison.process_error_ << ", " << comparison.error_value_ << ")  * "
               << error_value_multiplier_ << ")";
  score += temp_score;
  // stopper += 1;
  //}

  return score * multiplier_;
}

/**
 * Simulate observed values
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void Multinomial::SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  Double rng_uniform = 0.0;
  Double cumulative_expect = 0.0;
  Double check_error_val = 0.0;
  Double this_error_val = 0.0;

  auto iterator = comparisons.begin();
  vector<Double> expected_vals(iterator->second.size(), 0.0);
  vector<double> sim_obs(expected_vals.size(),0.0); 
  for (; iterator != comparisons.end(); ++iterator) {
    LOG_FINE() << "Simulating values for year: " << iterator->first;
    // clear these containers each year
    fill(expected_vals.begin(), expected_vals.end(), 0.0);
    fill(sim_obs.begin(), sim_obs.end(), 0.0);

    unsigned counter = 0;
    /*
    * Get error value for this year and expected value
    */
    for (observations::Comparison& comparison : iterator->second) {
      // bins
      expected_vals[counter] = comparison.expected_;
      if(counter == 0) {
        this_error_val = AdjustErrorValue(comparison.process_error_, comparison.error_value_);
      }
      check_error_val =  AdjustErrorValue(comparison.process_error_, comparison.error_value_);

      if(this_error_val != check_error_val)
        LOG_WARNING() << "Found N for multinomial in year " << iterator->first  << " had N = " << this_error_val << " which differend with the previous N = " << check_error_val << ". When simualting from multinomial need a single N for all bins in the composition. We are just using the first N and ignoreing the others"; 
      counter++;
    }
    // Now simulate using the uniform random variable
    while(this_error_val > 0) {
      rng_uniform = rng.uniform();
      cumulative_expect = 0.0;
      for (unsigned i = 0; i < expected_vals.size(); ++i) {
        cumulative_expect += expected_vals[i];
        if (cumulative_expect >= rng_uniform) {
          sim_obs[i]++;
          break;
        }
      }
      this_error_val--;
    }
    // Save back into the comparisons
    counter = 0;
    for (observations::Comparison& comparison : iterator->second) {
      comparison.observed_ = sim_obs[counter];
      counter++;
    }
  /*
    //    map<string, Double> totals;
    for (observations::Comparison& comparison : iterator->second) {
      Double error_value         = AdjustErrorValue(comparison.process_error_, comparison.error_value_);
      comparison.adjusted_error_ = error_value;

      if (comparison.expected_ <= 0.0 || error_value <= 0.0)
        comparison.observed_ = 0.0;
      else {
        LOG_FINEST() << "Expected = " << comparison.expected_;
        comparison.observed_ = rng.binomial(AS_DOUBLE(comparison.expected_), AS_DOUBLE(error_value));
        LOG_FINEST() << "Simulated = " << comparison.observed_;
      }
      //      totals[comparison.category_] += comparison.observed_;
    }
    */
    //    for (observations::Comparison& comparison : iterator->second)
    //      comparison.observed_ /= totals[comparison.category_];
  }
}

} /* namespace likelihoods */
} /* namespace niwa */
