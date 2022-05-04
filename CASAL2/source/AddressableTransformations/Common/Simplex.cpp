/**
 * @file Simplex.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 8, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 */

// headers
#include "Simplex.h"

#include "../../Estimates/Common/Uniform.h"
#include "../../Estimates/Estimate.h"
#include "../../Estimates/Manager.h"
#include "../../Model/Factory.h"
#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"

#include "../../Utilities/String.h"
#include "../../Utilities/To.h"
// namespaces
namespace niwa {
namespace addressabletransformations {
namespace utils = niwa::utilities;
/**
 * Default constructor
 */
Simplex::Simplex(shared_ptr<Model> model) : AddressableTransformation(model) {
  parameters_.Bind<bool>(PARAM_SUM_TO_ONE, &sum_to_one_, "if true parameter in natural space will sum = 1, otherwise they will sum = length(parameter)", "", true);
  RegisterAsAddressable(PARAM_SIMPLEX_PARAMETER, &simplex_parameter_); // equivalent to yk_ in the formulas
}

/**
 * Validate
 */
void Simplex::DoValidate() {
  LOG_FINE() << "Build values allocate memory";
  LOG_FINE() << "init_values_.size() " << init_values_.size() << " n-params " << n_params_;
  if(n_params_ <= 1) {
    LOG_FATAL_P(PARAM_PARAMETERS) << "simplex transformation needs a parameter with more than one element. Found " << n_params_ << " elements.";
  }
  LOG_FINE() << "cached values";
  restored_values_.resize(n_params_, 0.0);
  unit_vector_.resize(n_params_, 0.0);
  zk_.resize(n_params_ - 1, 0.0);
  cumulative_simplex_k_.resize(n_params_ - 1, 0.0);

  n_param_double_ = (Double)n_params_;
  Double count = 1.0;
  Km1_ = n_params_ - 1;
  if(prior_applies_to_restored_parameters_) {
    simplex_parameter_.resize(n_params_, 0.0);  // need to account for dummy estimate to apply prior to the nth parameter
    cached_simplex_values_for_objective_function_restore_.resize(n_params_, 0.0);
  } else {
    simplex_parameter_.resize(Km1_, 0.0);
  }

  cache_log_k_value_.resize(Km1_, 0.0);
  for(unsigned k = 0; k < cache_log_k_value_.size(); k++, count++) {
    cache_log_k_value_[k] = log(Km1_ - k);
    LOG_FINE( ) << " index = " << k << " count = " << count << " K = " << n_param_double_ << " cache K = " << cache_log_k_value_[k];
  }

  LOG_FINE() << "exit validate";
}

/**
 * Build
 */
void Simplex::DoBuild() {
  // get estimate label
  string estimate_label = "";
  // iterate over the 

  // iterate over the simplex estiamtes and check they are active, 
  // if prior applies to restored value, turn the last estimate to not estimate
  // but contribute to the objective function.
  unsigned n_par_in_estimates = n_params_;
  if(not prior_applies_to_restored_parameters_)
    n_par_in_estimates--;

  for(unsigned i = 0; i < n_par_in_estimates; ++i) {
    estimate_label = "parameter_transformation[" + label_ + "].simplex{" +  utilities::ToInline<unsigned, string>(i + 1) + "}";
    LOG_FINE() << "looking for " << estimate_label;
    simplex_estimate_ = model_->managers()->estimate()->GetEstimate(estimate_label);
    if(!simplex_estimate_) {
      LOG_WARNING() << "Could not find @estimate block for " << estimate_label << ", this is odd that you have a parameter transfomration, but not estimating the transformed parameter";
      continue;
    } else {
      // if last value and prior applies to restored value 
      if(prior_applies_to_restored_parameters_ & (i == (n_params_ - 1))) {
        LOG_FINE() << "setting last value to be a dummy prior for estimate " << estimate_label;
        simplex_estimate_->set_in_objective_function(true);
        simplex_estimate_->set_estimated(false);
        simplex_estimate_->set_mcmc_fixed(false);
      }
    }
    simplex_estimate_ = nullptr;
  }

  // check if parameter has been supplied by -i or -i Files
  if (IsAddressableUsedFor(PARAM_SIMPLEX_PARAMETER, addressable::kInputRun)) {
    simplex_input_supplied_ = true;
    LOG_FINE() << "simplex param given in -i " << simplex_input_supplied_;
  } else {
    LOG_FINE() << "simplex calculated using input config values";
  }
  // if given -i check they have supplied all the values otherwise error out.
  if(simplex_input_supplied_) {
    // calculated restored values
    Double stick_length = 1.0;
    for (unsigned k = 0; k < zk_.size(); ++k) {
      zk_[k] = utilities::math::invlogit(simplex_parameter_[k] - cache_log_k_value_[k]);
      unit_vector_[k] = stick_length * zk_[k];
      stick_length -= unit_vector_[k];
    }
    // plus group
    unit_vector_[unit_vector_.size() - 1] = stick_length;

    if(not sum_to_one_) {
      for(unsigned i = 0; i < restored_values_.size(); ++i) 
        restored_values_[i] = unit_vector_[i] * n_param_double_;
    }
  } else {
    // calculate the simplex
    total_ = 0.0;
    for(unsigned i = 0; i < init_values_.size(); ++i) {
      LOG_FINE() << init_values_[i];
      total_ += init_values_[i];
    }
    LOG_FINE() << "total = " << total_;
    if(sum_to_one_) {
      if(!utilities::math::IsOne(total_))
        LOG_ERROR_P(PARAM_PARAMETERS) << "You have specified a sum_to_one parameter but your parameters sum = " << total_;
      for(unsigned i = 0; i < init_values_.size(); ++i) {
        unit_vector_[i] = init_values_[i] / total_;
        LOG_FINE() << unit_vector_[i];
      }
    } else { 
      if(fabs(total_ - n_param_double_) > 0.0001) {
        LOG_WARNING() << "Values in your vector supplied should sum = " << n_param_double_ << " but sum = " << total_ << ", values returned will be rescaled to sum = " << n_param_double_;
        for(unsigned i = 0; i < init_values_.size(); ++i) {
          unit_vector_[i] = init_values_[i] / total_;
          LOG_FINE() << unit_vector_[i];
        }
        total_ = n_param_double_;
      } else {
        for(unsigned i = 0; i < init_values_.size(); ++i) {
          unit_vector_[i] = init_values_[i] / total_;
          LOG_FINE() << unit_vector_[i];
        }
      }
    }

    
    Double stick_length = unit_vector_[Km1_];
    LOG_FINE() << "Km1 = " << Km1_ << " stick length = " << stick_length;
    for (int k = Km1_; --k >= 0; ) {
      LOG_FINE() << "k = " << k;
      stick_length += unit_vector_[k];
      zk_[k] = unit_vector_[k] / stick_length;
      LOG_FINE() << "k = " << k << " zk_[k] " << zk_[k] << " stick length = " << stick_length << " cache_log_k_value_[k] = " << cache_log_k_value_[k];
      simplex_parameter_[k] =  utilities::math::logit(zk_[k]) + cache_log_k_value_[k];
      LOG_FINE() << "k = " << k;
      LOG_FINE() << "simplex = " << simplex_parameter_[k];
    }
  }
  LOG_FINE() << "exit Build";
}
/**
 * Restore
 * This method will restore values provided by the minimiser that need to be restored for use in the annual cycle
 */
void Simplex::DoRestore() {
  LOG_FINE() << label_;
  // calculated restored values
  Double stick_length = 1.0;
  Double adj_y_k = 0.0;
  jacobian_ = 0.0;
  // also calculates the log - jacobian in this step
  // see https://discourse.mc-stan.org/t/jacobian-of-the-unit-simplex-transform-and-transforms-between-spaces-of-unequal-dimensions/9934
  // For details on how the jacobian from Stan manual tranlates to the code below.
  for (int k = 0; k < Km1_; ++k) {
    adj_y_k = simplex_parameter_[k] - cache_log_k_value_[k];
    zk_[k] = utilities::math::invlogit(adj_y_k);
    LOG_FINE() << "k = " << k << " zk_[k] " << zk_[k] << " yk = " << simplex_parameter_[k] << " stick length = " << stick_length << " cache_log_k_value_[k] = " << cache_log_k_value_[k] << " adj_y_k = " << adj_y_k;
    // This will calculate the log-probability for the jacobian
    jacobian_ += log(stick_length);
    jacobian_ -= utilities::math::log1p_exp(-adj_y_k);
    jacobian_ -= utilities::math::log1p_exp(adj_y_k);
    //LOG_FINE() << "log jacobian " << jacobian_;
    unit_vector_[k] = stick_length * zk_[k];
    stick_length -= unit_vector_[k];
  }
  // plus group
  unit_vector_[unit_vector_.size() - 1] = stick_length;
  if(not sum_to_one_) {
    for(unsigned i = 0; i < restored_values_.size(); ++i) {
      restored_values_[i] = unit_vector_[i] * total_;
      LOG_FINE() << unit_vector_[i] << "  " << restored_values_[i];
    }
    (this->*restore_function_)(restored_values_);
  } else {
    (this->*restore_function_)(unit_vector_);
  }
}

/**
 *  Calculate the Jacobian, to offset the bias of the transformation that enters the objective function
 */
Double Simplex::GetScore() {
  LOG_TRACE();
  if(not prior_applies_to_restored_parameters_)
    return 0.0;
  // The jacobian is calculate in the DoRestore();
  LOG_FINE() << jacobian_;
  return -1.0 * jacobian_; // return negative log-likelihood
}
/**
 * PrepareForObjectiveFunction
 * if prior_applies_to_restored_parameters_
 */
void Simplex::PrepareForObjectiveFunction() {
  LOG_FINE();
  if(prior_applies_to_restored_parameters_) {
    for(unsigned i = 0; i < simplex_parameter_.size(); ++i) {
      cached_simplex_values_for_objective_function_restore_[i] = simplex_parameter_[i];
      simplex_parameter_[i] = restored_values_[i];
    }
  }
}

/**
 * RestoreForObjectiveFunction
 * if prior_applies_to_restored_parameters_
 */
void Simplex::RestoreForObjectiveFunction() {
  LOG_FINE();
  if(prior_applies_to_restored_parameters_) {
    for(unsigned i = 0; i < simplex_parameter_.size(); ++i) 
      simplex_parameter_[i] = cached_simplex_values_for_objective_function_restore_[i];
  }
}

/**
 * Report stuff for this transformation
 */
void Simplex::FillReportCache(ostringstream& cache) {
  LOG_FINE() << "FillReportCache";
  cache << PARAM_PARAMETERS << ": ";
  for(unsigned i = 0; i < parameter_labels_.size(); ++i)
    cache << parameter_labels_[i] << " ";
  cache << REPORT_EOL;
  cache << "simplex_values: ";
  for(unsigned i = 0; i < simplex_parameter_.size(); ++i)
    cache << simplex_parameter_[i] << " ";
  cache << REPORT_EOL;
  cache << "parameter_values: ";
  for(unsigned i = 0; i < restored_values_.size(); ++i)
    cache << restored_values_[i] << " ";
  cache << REPORT_EOL;
  cache << "negative_log_jacobian: " << jacobian_ << REPORT_EOL;
}


/**
 * Report stuff for this transformation
 */
void Simplex::FillTabularReportCache(ostringstream& cache, bool first_run)  {
  LOG_FINEST() << "FillTabularReportCache";
  if(first_run) {
    for(unsigned i = 0; i < zk_.size(); ++i)
      cache << simplex_parameter_[i] << " ";
    for(unsigned i = 0; i < restored_values_.size(); ++i)
      cache << restored_values_[i] << " ";
    cache << "negative_log_jacobian" << REPORT_EOL;
  }
  for(unsigned i = 0; i < zk_.size(); ++i)
    cache << "simplex_value[" << i + 1 << "] ";
  for(unsigned i = 0; i < restored_values_.size(); ++i)
    cache << "parameter_value[" << i + 1 << "] ";
  cache << jacobian_ << REPORT_EOL;
}

} /* namespace addressabletransformations */
} /* namespace niwa */
