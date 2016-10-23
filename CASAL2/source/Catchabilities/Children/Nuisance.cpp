/**
 * @file nuisance.cpp
 * @author C.Marsh
 * @github https://github.com/NIWAFisheriesModelling
 * @date 16/08/2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */

// headers
#include "Catchabilities/Children/Nuisance.h"

#include "Estimates/Manager.h"
#include "Estimates/Estimate.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace catchabilities {

namespace dc = niwa::utilities::doublecompare;
namespace utils = niwa::utilities;

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g profiling, yields, projections etc)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate Latex for the documentation.
 */
Nuisance::Nuisance(Model* model) : Catchability(model) {
  RegisterAsEstimable(PARAM_Q, &q_);
}

/*
 *
 */
void Nuisance::DoValidate() {
}

/*
 *  Build
 */

void Nuisance::DoBuild() {

  LOG_TRACE();

  //This was the first path that I went down, the second was going down an additional prior.
  string parameter = "catchability["+ label_+"].q";
  // Find out if an estimate exists
  LOG_FINEST() << "Find estimate block for parameter " << parameter;

  bool is_estimated;

  is_estimated = model_->managers().estimate()->HasEstimate(parameter);

  if (is_estimated) {
    // Obtain a pointer to the estimate
    Estimate* estimate = model_->managers().estimate()->GetEstimate(parameter);
    if (!estimate)
      LOG_ERROR() << "Can not get estimate with the parameter name " << parameter;
    // Find out the prior type
    prior_ = estimate->type();
    LOG_FINEST() << "Type of prior on Nuisance Q = "  << prior_;

    // Get the lower and upper bounds
    lower_bound_ = estimate->lower_bound();
    upper_bound_ = estimate->upper_bound();
    // Perhaps set value to the mean of the bounds for now if the estimate system cannot handle an uninitialised estimate
    q_ = (upper_bound_ + lower_bound_) / 2.0;


    if (prior_ == PARAM_LOGNORMAL) {
    // Need to store mu and cv for the prior for use later
    map<string, Parameter*> parameters = estimate->parameters().parameters();
     for (auto iter = parameters.begin(); iter != parameters.end(); ++iter) {
       if (iter->first == PARAM_MU) {
         Double mu = 0.0;
         for (string parameter_value : iter->second->values()) {
           if (!utils::To<Double>(parameter_value, mu))
             LOG_ERROR() << "parameter mu = " << parameter_value << " cannot be converted to a double";
         }
         mu_ = mu;
       }
       if (iter->first == PARAM_CV) {
         Double cv = 0.0;
         for (string parameter_value : iter->second->values()) {
           if (!utils::To<Double>(parameter_value, cv))
             LOG_ERROR() << "parameter cv = " << parameter_value << " cannot be converted to a double";
         }
         cv_ = cv;
       }
     }
    }


    // Turn off estimation but turn on contribute to the objective function
    estimate->set_in_objective_function(true);
    estimate->set_estimated(false);

  } else {
    LOG_FINEST() << "solving for q in a maximum likelihood context. i.e. no prior";
    q_ = 1.0;
    LOG_WARNING() << "No @estimate block found for this nuisance q. It is advised to have a bounds on q as it constrains the analytical solution from going into unrealistic space.";
  }


}

/**
 * Function CalculateQ() Analytically solve for Q, this gets called in Observation/Children/Biomass.cpp and Abundance.cpp
 *
 * @param Comparison this is the comparsion object that observed, expected and errors are stored in the Observation class.
 * @param likelihood is a string indicating the type of likelihood
 *
 * @return this function will solve for q given the comparison structure.
 */
void Nuisance::CalculateQ(map<unsigned, vector<observations::Comparison> >& comparisons,const string& likelihood) {
  LOG_TRACE();
  LOG_FINEST() << "Converting nuisance q with prior = " << prior_ << " and likelihood = " << likelihood;
  if (likelihood != PARAM_NORMAL && likelihood != PARAM_LOGNORMAL) {
    LOG_FATAL() << "Nuisance q method can only be applied to observations that have the following likelihoods, normal and "
        "lognormal. This needs to be corrected in the @observation block, or alternatively try using a q type = free";
  }

  // The first set of conditions
  if (likelihood == PARAM_NORMAL && (prior_ == PARAM_NONE || prior_ == PARAM_UNIFORM)){
    // This syntax follows the manual
    Double s1 = 0, s2 = 0;
    double n = 0;
    // iterate over each year
    for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
      // Iterate over each category
      for (observations::Comparison& comparison : year_iterator->second) {

        Double cv = comparison.error_value_;
        if (comparison.error_value_ > 0.0 && comparison.process_error_ > 0.0)
          cv =  sqrt(comparison.error_value_ * comparison.error_value_ + comparison.process_error_ * comparison.process_error_);

        s1 += comparison.observed_ / (cv * cv * comparison.expected_);
        s2 += pow(comparison.observed_ / (cv * comparison.expected_), 2);
        n++;
      }
    }
    q_ = (-s1 + sqrt(s1 * s1 + 4 * n * s2)) / (2 * n);


  } else if (likelihood == PARAM_LOGNORMAL && (prior_ == PARAM_NONE || prior_ == PARAM_UNIFORM)){
  Double s3 = 0, s4 = 0;
  double n = 0;
  for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
    // Iterate over each category
    for (observations::Comparison& comparison : year_iterator->second) {
      if (comparison.expected_ <= ZERO)
        LOG_WARNING() << "Expected less than " << ZERO << " you may want to check this.";
      comparison.expected_ = dc::ZeroFun(comparison.expected_,ZERO);
        n++;
        Double cv = comparison.error_value_;
        if (comparison.error_value_ > 0.0 && comparison.process_error_ > 0.0)
          cv =  sqrt(comparison.error_value_ * comparison.error_value_ + comparison.process_error_ * comparison.process_error_);

        Double sigma = sqrt(log(1 + cv * cv));

        s3 += log(comparison.observed_ / comparison.expected_)/ (sigma * sigma);
        s4 += 1 / (sigma * sigma);
      }
    }
  LOG_FINEST() << "S3 = " << s3 << " S4 = " << s4;
  q_ = exp((0.5 * n + s3) / s4);

  } else if (likelihood == PARAM_NORMAL && prior_ == PARAM_UNIFORM_LOG){
    Double s1 = 0, s2 = 0;
    double n = 0;
    for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
      // Iterate over each category
      for (observations::Comparison& comparison : year_iterator->second) {
        comparison.expected_ = dc::ZeroFun(comparison.expected_,ZERO);
          n++;
          Double cv = comparison.error_value_;
          if (comparison.error_value_ > 0.0 && comparison.process_error_ > 0.0)
            cv =  sqrt(comparison.error_value_ * comparison.error_value_ + comparison.process_error_ * comparison.process_error_);

          s1 += comparison.observed_ / (cv * cv * comparison.expected_);
          s2 += pow(comparison.observed_ / (cv * comparison.expected_), 2);
        }
      }
      LOG_FINEST() << "S1 = " << s1 << " S2 = " << s2;
      q_ = (-s1 + sqrt(s1 * s1 + 4 * (n + 1) * s2)) / (2 * (n + 1));

  } else if (likelihood == PARAM_LOGNORMAL && prior_ == PARAM_UNIFORM_LOG) {
    Double s3 = 0, s4 = 0;
    double n = 0;
    for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
      // Iterate over each category
      for (observations::Comparison& comparison : year_iterator->second) {
        comparison.expected_ = dc::ZeroFun(comparison.expected_,ZERO);
          n++;
          Double cv = comparison.error_value_;
          if (comparison.error_value_ > 0.0 && comparison.process_error_ > 0.0)
            cv =  sqrt(comparison.error_value_ * comparison.error_value_ + comparison.process_error_ * comparison.process_error_);


          Double sigma = sqrt(log(1 + cv * cv));
          LOG_FINEST() << "Sigma = " << sigma;
          LOG_FINEST() << "E = " << comparison.expected_;
          s3 += log(comparison.observed_ /  comparison.expected_) / (sigma * sigma);
          s4 += 1 / (sigma * sigma);
        }
      }
    LOG_FINEST() << "S3 = " << s3 << " S4 = " << s4;

    q_ = exp((0.5 * n - 1 + s3) / s4);
    } else if (likelihood == PARAM_LOGNORMAL && prior_ == PARAM_LOGNORMAL){
      Double s3 = 0, s4 = 0;
      double n = 0;
      for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
        // Iterate over each category
        for (observations::Comparison& comparison : year_iterator->second) {
          comparison.expected_ = dc::ZeroFun(comparison.expected_,ZERO);
            n++;
            Double cv = comparison.error_value_;
            if (comparison.error_value_ > 0.0 && comparison.process_error_ > 0.0)
              cv =  sqrt(comparison.error_value_ * comparison.error_value_ + comparison.process_error_ * comparison.process_error_);

            Double sigma = sqrt(log(1 + cv * cv));
            LOG_FINEST() << "obs = " <<  comparison.observed_ << " exp = " <<  comparison.expected_ << " sigma = " << sigma;

            s3 += log(comparison.observed_ / comparison.expected_) / (sigma * sigma);
            s4 += 1 / (sigma * sigma);
          }
        }
      Double var_q = log(1 + cv_ * cv_);
      LOG_FINE() << "mu = " << mu_ << " cv = " << cv_ << " s3 = " << s3 << " s4 = " << s4 << " n = " << n;
      q_ = exp((0.5 * n - 1.5 + log(mu_) / var_q + s3) / (s4 + 1 / var_q));
    } else {
      LOG_ERROR() << "Unrecognised combination in CalculateQ : likelihood_type = " <<  likelihood << " prior_type = " << prior_ << " these combinations ";
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
