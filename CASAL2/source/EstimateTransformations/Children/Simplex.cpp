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
  parameters_.Bind<string>(PARAM_ESTIMATE, &estimate_label_, "Estimate to transform", "");
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
  estimates_ = model_->managers().estimate()->GetEstimatesByLabel(estimate_label_);
  if (estimates_.size() < 1) {
    LOG_ERROR_P(PARAM_ESTIMATE) << "Estimate " << estimate_label_ << " could not be found. Have you defined it?";
    return;
  }
  length_ = estimates_.size();
  simplex_values_.assign((length_ - 1), 0.0);
  // Populate the simplex vector
  for (unsigned i = 0; i < length_ - 1; ++i) {
    Double value = estimates_[i]->value();
    total_ += value;
    simplex_values_[i] = value;
  }
  total_ += estimates_[length_]->value();

  // Create, populate and validate simplex estimates
  for (unsigned i = 0; i < length_ - 1; ++i) {
    estimates::Uniform* simplex = new estimates::Uniform();
    simplex->set_block_type(PARAM_ESTIMATE);
    simplex->set_defined_file_name(__FILE__);
    simplex->set_defined_line_number(__LINE__);
    simplex->parameters().Add(PARAM_LABEL, "simplex_" + i, __FILE__, __LINE__);
    simplex->set_in_objective_function(false);
    simplex->set_target(&simplex_values_[i]);
    simplex->set_creator_parameter("simplex_" + i);
    //simplex->set_lower_bound(x);    // Ask Alistair what the bounds be
    //simplex->set_upper_bound(y);    //
    simplex->Validate();
    model_->managers().estimate()->AddObject(simplex);
  }
}

/**
 *
 */
void Simplex::Transform() {
  // disable original estimate
  // enable pseudo estimate switching all switches neccassary for the transformation
  // set upper and lower bounds

}

/**
 *
 */
void Simplex::Restore() {
  // disable pseudo estimate and enable original parameter
  // change bounds back

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
