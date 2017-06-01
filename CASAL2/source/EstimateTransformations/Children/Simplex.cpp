/**
 * @file Simplex.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 8, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */

// headers
#include "Simplex.h"

#include "Model/Model.h"
#include "Model/Objects.h"
#include "Model/Managers.h"
#include "Model/Factory.h"
#include "Estimates/Manager.h"
#include "Estimates/Estimate.h"
#include "Estimates/Children/Uniform.h"

// namespaces
namespace niwa {
namespace estimatetransformations {
namespace utils = niwa::utilities;
/**
 * Default constructor
 */
Simplex::Simplex(Model* model) : EstimateTransformation(model) {
  parameters_.Bind<string>(PARAM_ESTIMATE, &estimate_label_, "The label for the estimate label to use in the simplex transformation", "");

  is_simple_ = true;
}

/**
 */
void Simplex::DoValidate() {

}

/**
 *
 */
void Simplex::DoBuild() {
  LOG_TRACE();
  LOG_WARNING() << "Simplex transforamtion works but is version 1.0 may need more work for calculating bounds";
  estimates_ = model_->managers().estimate()->GetEstimatesByLabel(estimate_label_);
  if (estimates_.size() < 1) {
    LOG_ERROR_P(PARAM_ESTIMATE) << "Estimate " << estimate_label_ << " could not be found. Have you defined it?";
    return;
  }

  // Validate
  length_ = estimates_.size();
  simplex_values_.assign((length_ - 1), 0.0);

  // Check that the vector sums or averages to one
  for (unsigned i = 0; i < estimates_.size(); ++i) {
    total_ += estimates_[i]->value();
    estimates_[i]->set_estimated(false);
  }
  LOG_MEDIUM() << "total = " << total_ << " length " << length_;

  // Do a validation if vector doesn't sum to 1 of length (number of elements) error can't use this method
  if (fabs(1.0 - total_) > 0.001 && (total_ - (length_)) > 0.001)
   LOG_ERROR_P(PARAM_ESTIMATE) << "This transformation can only be used on vectors that have a mean = 1 or sum = 1";

  // Populate the simplex vector
  for (unsigned i = 0; i < simplex_values_.size(); ++i)
    sub_total_ += estimates_[i]->value();

  for (unsigned i = 0; i < simplex_values_.size(); ++i) {
    simplex_values_[i] = estimates_[i]->value() / (total_ - sub_total_);
  }

  LOG_MEDIUM() << "sub_total = " << sub_total_;

  Double L_bound = estimates_[length_ - 1]->lower_bound();
  Double U_bound = estimates_[length_ - 1]->upper_bound();
  // Create, populate and validate simplex estimates
  for (unsigned i = 0; i < simplex_values_.size(); ++i) {
    estimates::Uniform* simplex = new estimates::Uniform(model_);
    simplex->set_block_type(PARAM_ESTIMATE);
    simplex->set_defined_file_name(__FILE__);
    simplex->set_defined_line_number(__LINE__);

    string element;
    if (!utilities::To<unsigned, string>(i, element))
      LOG_ERROR() << i <<  " could not be converted to type sting";
    string param_label = "simplex_" + element;

    simplex->set_label(param_label);
    simplex->set_target(&simplex_values_[i]);
    simplex->set_creator_parameter(param_label);
    simplex->set_lower_bound(estimates_[i]->lower_bound()  / U_bound);
    simplex->set_upper_bound(estimates_[i]->upper_bound()  / L_bound);
    LOG_MEDIUM() << "value = " << simplex->value() << ", upper bound = " << simplex->upper_bound() << ", lower bound = " << simplex->lower_bound();
    simplex->set_in_objective_function(false);
    simplex_estimates_.push_back(simplex);
    model_->managers().estimate()->AddObject(simplex);
  }
}

/**
 *
 */
void Simplex::Transform() {

}

/**
 *    This will restore values provided by the minimiser that need to be restored for use in the annual cycle
 */
void Simplex::Restore() {
  LOG_TRACE();
  // intialise as 1 which accounts for the known parameter
  Double new_total = 1.0;

  for (unsigned i = 0; i < simplex_values_.size(); ++i) {
    new_total += simplex_values_[i];
  }
  LOG_MEDIUM() << "total: " << new_total;


  // Calculate new values for annual cycle whilst reversing the relevant
  for (unsigned i = 0; i < simplex_values_.size(); ++i) {
    Double restored_value = (simplex_values_[i] / new_total * total_);
    LOG_MEDIUM() << "Parameter label: " << estimates_[i]->parameter() << "Restored Value = " << restored_value;
    estimates_[i]->set_value(restored_value);
  }
  Double restore_final_value = (1.0 / new_total * total_);
  estimates_[length_ - 1]->set_value(restore_final_value);
  LOG_MEDIUM() << "Parameter label: " << estimates_[length_ - 1]->parameter() << "Restored Value = " << restore_final_value;
}

/**
 *  Calculate the Jacobian, to offset the bias of the transformation that enters the objective function
 */
Double Simplex::GetScore() {
  jacobian_ = simplex_values_[0] * (1 - simplex_values_[0]);
  if (simplex_values_.size() > 1 ) {
  for (unsigned i = 1; i < simplex_values_.size(); ++i) {
    LOG_MEDIUM() << "val = " << simplex_values_[i];
    jacobian_ *= simplex_values_[i] * (1 - simplex_values_[i]);
    }
  }
  jacobian_ *= (total_ - sub_total_);
  LOG_MEDIUM() << "Jacobian: " << jacobian_;
  return jacobian_;
}

/**
 * Get the target estimables so we can ensure each
 * object is not referencing multiple ones as this would
 * cause chain issues
 *
 * @return Set of estimable labels
 */
std::set<string> Simplex::GetTargetEstimates() {
  set<string> result;
  result.insert(estimate_label_);
  return result;
}
} /* namespace estimatetransformations */
} /* namespace niwa */
