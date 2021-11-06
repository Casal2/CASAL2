/**
 * @file nuisance.cpp
 * @author C.Marsh
 * @github https://github.com/NIWAFisheriesModelling
 * @date 16/08/2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 */

// headers
#include "Nuisance.h"

#include "../../AdditionalPriors/Manager.h"
#include "../../Utilities/Math.h"
#include "../../Utilities/To.h"

// namespaces
namespace niwa {
namespace catchabilities {

namespace math  = niwa::utilities::math;
namespace utils = niwa::utilities;

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g., profiling, yields, projections, etc.)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate LaTeX for the documentation.
 */
Nuisance::Nuisance(shared_ptr<Model> model) : Catchability(model) {
  parameters_.Bind<Double>(PARAM_LOWER_BOUND, &lower_bound_, "The upper bound for nuisance catchability", "");
  parameters_.Bind<Double>(PARAM_UPPER_BOUND, &upper_bound_, "The lower bound for nuisance catchability", "");

  RegisterAsAddressable(PARAM_Q, &q_, addressable::kAll);
}

/**
 * Validate the objects
 */
void Nuisance::DoBuild() {
  // TODO: Fix this. Building dynamic parameter feels horrible.
  if (IsAddressableUsedFor(PARAM_Q, addressable::kEstimate)) 
      LOG_ERROR_P(PARAM_Q) <<"Has an @esimate block, this is not allowed. We suggest adding an @additional_prior for this nuisance parameter";
  

  /**
   *  Build the objects
   */
  LOG_TRACE();

  // This was the first path that I went down, the second was going down an additional prior.
  string parameter = "catchability[" + label_ + "].q";
  // Find out if an estimate exists
  LOG_FINEST() << "Find an @additional_prior command for parameter " << parameter;

  bool has_prior = false;
  has_prior = model_->managers()->additional_prior()->HasAdditionalPrior(parameter);

  if (has_prior) {
    // Obtain a pointer to the estimate
    AdditionalPrior* additional_prior = model_->managers()->additional_prior()->GetAdditionalPrior(parameter);
    if (!additional_prior)
      LOG_ERROR() << "Can not get additional prior with the parameter label " << parameter;
    // Find out the prior type
    prior_type_ = additional_prior->type();
    LOG_FINEST() << "Type of prior on Nuisance q = " << prior_type_;

    if(prior_type_ != PARAM_LOGNORMAL && prior_type_ != PARAM_NONE && prior_type_ != PARAM_UNIFORM_LOG)
      LOG_ERROR_P(PARAM_LABEL) << "the additional prior type needs to be either 'none', 'lognormal' or 'uniform_log'";

    // Perhaps set value to the mean of the bounds for now if the estimate system cannot handle an uninitialised estimate
    q_ = (upper_bound_ + lower_bound_) / 2.0;

    if (prior_type_ == PARAM_LOGNORMAL) {
      // Need to store mu and cv for the prior for use later
      map<string, Parameter*> parameters = additional_prior->parameters().parameters();
      for (auto iter = parameters.begin(); iter != parameters.end(); ++iter) {
        if (iter->first == PARAM_MU) {
          Double mu = 0.0;
          for (string parameter_value : iter->second->values()) {
            if (!utils::To<Double>(parameter_value, mu))
              LOG_ERROR() << "parameter mu = " << parameter_value << " could not be converted to a double";
          }
          mu_ = mu;
        }
        if (iter->first == PARAM_CV) {
          Double cv = 0.0;
          for (string parameter_value : iter->second->values()) {
            if (!utils::To<Double>(parameter_value, cv))
              LOG_ERROR() << "parameter CV = " << parameter_value << " could not be converted to a double";
          }
          cv_ = cv;
        }
      }
    }
  } else {
    LOG_FINEST() << "solving for the nuisance q in a maximum likelihood context, i.e., with no prior";
    q_ = 1.0;
  }
}

/**
 * Function CalculateQ() Analytically solve for Q, this gets called in Observation/Common/Biomass.cpp and Abundance.cpp
 *
 * @param Comparison this is the comparison object that observed, expected and errors are stored in the Observation class.
 * @param likelihood is a string indicating the type of likelihood
 *
 * @return this function will solve for q given the comparison structure.
 */
void Nuisance::CalculateQ(map<unsigned, vector<observations::Comparison> >& comparisons, string_view likelihood) {
  LOG_TRACE();
  LOG_FINEST() << "Converting nuisance q with prior = " << prior_type_ << " and likelihood = " << likelihood;
  if (likelihood != PARAM_NORMAL && likelihood != PARAM_LOGNORMAL) {
    LOG_FATAL() << "The nuisance q method can be applied only to observations with normal or lognormal likelihoods. "
                << "Check the observation likelihood or use type = free";
  }

  // The first set of conditions
  if (likelihood == PARAM_NORMAL && (prior_type_ == PARAM_NONE)) {
    // This syntax follows the manual
    Double s1 = 0, s2 = 0;
    double n = 0;
    // iterate over each year
    for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
      // Iterate over each category
      for (observations::Comparison& comparison : year_iterator->second) {
        Double cv = comparison.error_value_;
        if (comparison.error_value_ > 0.0 && comparison.process_error_ > 0.0)
          cv = sqrt(comparison.error_value_ * comparison.error_value_ + comparison.process_error_ * comparison.process_error_);

        s1 += comparison.observed_ / (cv * cv * comparison.expected_);
        s2 += pow(comparison.observed_ / (cv * comparison.expected_), 2);
        n++;
      }
    }
    q_ = (-s1 + sqrt(s1 * s1 + 4 * n * s2)) / (2 * n);

  } else if (likelihood == PARAM_LOGNORMAL && (prior_type_ == PARAM_NONE)) {
    Double s3 = 0, s4 = 0;
    double n = 0;
    for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
      // Iterate over each category
      for (observations::Comparison& comparison : year_iterator->second) {
        if (comparison.expected_ <= math::ZERO)
          LOG_WARNING() << "The comparison expected less than " << math::ZERO;
        comparison.expected_ = math::ZeroFun(comparison.expected_, math::ZERO);
        n++;
        Double cv = comparison.error_value_;
        if (comparison.error_value_ > 0.0 && comparison.process_error_ > 0.0)
          cv = sqrt(comparison.error_value_ * comparison.error_value_ + comparison.process_error_ * comparison.process_error_);

        Double sigma = sqrt(log(1 + cv * cv));

        s3 += log(comparison.observed_ / comparison.expected_) / (sigma * sigma);
        s4 += 1 / (sigma * sigma);
      }
    }
    LOG_FINEST() << "S3 = " << s3 << " S4 = " << s4;
    q_ = exp((0.5 * n + s3) / s4);

  } else if (likelihood == PARAM_NORMAL && prior_type_ == PARAM_UNIFORM_LOG) {
    Double s1 = 0, s2 = 0;
    double n = 0;
    for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
      // Iterate over each category
      for (observations::Comparison& comparison : year_iterator->second) {
        comparison.expected_ = math::ZeroFun(comparison.expected_, math::ZERO);
        n++;
        Double cv = comparison.error_value_;
        if (comparison.error_value_ > 0.0 && comparison.process_error_ > 0.0)
          cv = sqrt(comparison.error_value_ * comparison.error_value_ + comparison.process_error_ * comparison.process_error_);

        s1 += comparison.observed_ / (cv * cv * comparison.expected_);
        s2 += pow(comparison.observed_ / (cv * comparison.expected_), 2);
      }
    }
    LOG_FINEST() << "S1 = " << s1 << " S2 = " << s2;
    q_ = (-s1 + sqrt(s1 * s1 + 4 * (n + 1) * s2)) / (2 * (n + 1));

  } else if (likelihood == PARAM_LOGNORMAL && prior_type_ == PARAM_UNIFORM_LOG) {
    Double s3 = 0, s4 = 0;
    double n = 0;
    for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
      // Iterate over each category
      for (observations::Comparison& comparison : year_iterator->second) {
        comparison.expected_ = math::ZeroFun(comparison.expected_, math::ZERO);
        n++;
        Double cv = comparison.error_value_;
        if (comparison.error_value_ > 0.0 && comparison.process_error_ > 0.0)
          cv = sqrt(comparison.error_value_ * comparison.error_value_ + comparison.process_error_ * comparison.process_error_);

        Double sigma = sqrt(log(1 + cv * cv));
        LOG_FINEST() << "Sigma = " << sigma;
        LOG_FINEST() << "E = " << comparison.expected_;
        s3 += log(comparison.observed_ / comparison.expected_) / (sigma * sigma);
        s4 += 1 / (sigma * sigma);
      }
    }
    LOG_FINEST() << "S3 = " << s3 << " S4 = " << s4;

    q_ = exp((0.5 * n - 1 + s3) / s4);
  } else if (likelihood == PARAM_LOGNORMAL && prior_type_ == PARAM_LOGNORMAL) {
    Double s3 = 0, s4 = 0;
    double n = 0;
    for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
      // Iterate over each category
      for (observations::Comparison& comparison : year_iterator->second) {
        comparison.expected_ = math::ZeroFun(comparison.expected_, math::ZERO);
        n++;
        Double cv = comparison.error_value_;
        if (comparison.error_value_ > 0.0 && comparison.process_error_ > 0.0)
          cv = sqrt(comparison.error_value_ * comparison.error_value_ + comparison.process_error_ * comparison.process_error_);

        Double sigma = sqrt(log(1 + cv * cv));
        LOG_FINEST() << "obs = " << comparison.observed_ << " exp = " << comparison.expected_ << " sigma = " << sigma;

        s3 += log(comparison.observed_ / comparison.expected_) / (sigma * sigma);
        s4 += 1 / (sigma * sigma);
      }
    }
    Double var_q = log(1 + cv_ * cv_);
    LOG_FINE() << "mu = " << mu_ << " cv = " << cv_ << " s3 = " << s3 << " s4 = " << s4 << " n = " << n;
    q_ = exp((0.5 * n - 1.5 + log(mu_) / var_q + s3) / (s4 + 1 / var_q));
  } else {
    LOG_ERROR() << "Unrecognised combination in CalculateQ : likelihood_type = " << likelihood << " prior_type = " << prior_type_;
  }

  LOG_FINE() << "Analytical q = " << q_;

  if (q_ > upper_bound_) {
    q_ = upper_bound_;
    LOG_FINE() << "Nuisance q hit upper bound q set to upper bound = " << upper_bound_;
  }

  if (q_ < lower_bound_) {
    q_ = lower_bound_;
    LOG_FINE() << "Nuisance q hit upper bound q set to lower bound = " << lower_bound_;
  }
  LOG_FINEST() << "Setting q = " << q_;
}

} /* namespace catchabilities */
} /* namespace niwa */
