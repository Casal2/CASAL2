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

// namespaces
namespace niwa {
namespace addressabletransformations {
namespace utils = niwa::utilities;
/**
 * Default constructor
 */
Simplex::Simplex(shared_ptr<Model> model) : AddressableTransformation(model) {
  parameters_.Bind<bool>(PARAM_SUM_TO_ONE, &sum_to_one_, "if true parameter in natural space will sum = 1, otherwise they will sum = length(parameter)", "", true);

}

/**
 * Validate
 */
void Simplex::DoValidate() {
  LOG_FINE() << "Build values allocate memory";
  LOG_FINE() << "init_values_.size() " << init_values_.size() << " n-params " << n_params_;

  n_param_double_ = (Double)n_params_;
  Double count = 1.0;
  
  cache_log_k_value_.resize(n_params_ - 1, 0.0);
  for(unsigned i = 0; i < init_values_.size(); ++i) {
    LOG_FINE() << init_values_[i];
    if(i < (n_params_ - 1))
      cache_log_k_value_[i] = log(1.0 / (n_param_double_ - count));
  }
  restored_values_.resize(n_params_, 0.0);
  zk_.resize(n_params_ - 1, 0.0);
  cumulative_simplex_k_.resize(n_params_ - 1, 0.0);
  // TODO Scott, need to know if simplex_parameter_ is input in -i 
  // if given -i check they have supplied all the values otherwise error out.
  for(unsigned i = 0; i < init_values_.size(); ++i) {
    total_ += init_values_[i];
  }
  if(sum_to_one_) {
    if((total_ - 1.0) > 0.0001)
      LOG_ERROR_P(PARAM_PARAMETERS) << "You have specified a sum_to_one parameter but your parameters = " << sum_to_one_;
  }

  for(unsigned i = 0; i < init_values_.size(); ++i) {
    unit_vector_[i] = init_values_[i] / total_;
  }
  // TODO Scott, need to know if simplex_parameter_ is input in -i 
  // if given -i restore  simplex_parameter_ -> restore_values_
  // this maps yk_ -> zk_
  for (unsigned i = 0; i < simplex_parameter_.size(); ++i) {
    zk_[i] = 1.0 / (1.0 + exp(-simplex_parameter_[i] + cache_log_k_value_[i]));
  }
  // Translate zk_ -> restore_values_
  sub_total_ = 0;
  for (unsigned i = 0; i < zk_.size(); ++i) {
    cumulative_simplex_k_[i] = sub_total_;
    unit_vector_[i] = (1 - sub_total_) * zk_[i];
    sub_total_ += unit_vector_[i];
  }
  // plus group
  unit_vector_[unit_vector_.size() - 1] = 1.0 - sub_total_;

  if(not sum_to_one_) {
    for(unsigned i = 0; i < restored_values_.size(); ++i) 
      restored_values_[i] = unit_vector_[i] * total_;
  }
  RegisterAsAddressable(PARAM_SIMPLEX_PARAMETER, &simplex_parameter_); // equivalent to yk_ in the formulas

}

/**
 * Build
 */
void Simplex::DoBuild() {

}
/**
 * Restore
 * This method will restore values provided by the minimiser that need to be restored for use in the annual cycle
 */
void Simplex::DoRestore() {
  LOG_FINE() << "";
  // this maps yk_ -> zk_
  for (unsigned i = 0; i < simplex_parameter_.size(); ++i) {
    zk_[i] = 1.0 / (1.0 + exp(-simplex_parameter_[i] + cache_log_k_value_[i]));
  }
  // Translate zk_ -> restore_values_
  sub_total_ = 0;
  for (unsigned i = 0; i < zk_.size(); ++i) {
    unit_vector_[i] = (1 - sub_total_) * zk_[i];
    sub_total_ += unit_vector_[i];
  }
  // plus group
  unit_vector_[unit_vector_.size() - 1] = 1.0 - sub_total_;

  if(not sum_to_one_) {
    for(unsigned i = 0; i < restored_values_.size(); ++i) 
      restored_values_[i] = unit_vector_[i] * total_;
  }
  for(unsigned i = 0; i < restored_values_.size(); ++i)
    LOG_FINE() << restored_values_[i];
  (this->*restore_function_)(restored_values_);

}

/**
 *  Calculate the Jacobian, to offset the bias of the transformation that enters the objective function
 */
Double Simplex::GetScore() {
  LOG_TRACE();
  if(not prior_applies_to_restored_parameters_)
    return 0.0;

  jacobian_ = 1.0;
  for (unsigned i = 0; i < zk_.size(); ++i)
    jacobian_ *= zk_[i] * ( 1.0 - zk_[i]) * (1 - cumulative_simplex_k_[i]);
  return -1.0 * log(jacobian_); // return negative log-likelihood
}
/**
 * PrepareForObjectiveFunction
 * if prior_applies_to_restored_parameters_
 */
void Simplex::PrepareForObjectiveFunction() {
}

/**
 * RestoreForObjectiveFunction
 * if prior_applies_to_restored_parameters_
 */
void Simplex::RestoreForObjectiveFunction() {
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
  cache << "parameter_values: ";
  for(unsigned i = 0; i < restored_values_.size(); ++i)
    cache << restored_values_[i] << " ";
  cache << "negative_log_jacobian: " << jacobian_ << REPORT_EOL;

}

} /* namespace addressabletransformations */
} /* namespace niwa */
