/**
 * @file VectorSmoothing.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "VectorSmoothing.h"

#include "Estimates/Manager.h"
#include "ObjectsFinder/ObjectsFinder.h"

// namespaces
namespace isam {
namespace penalties {

/**
 * Default constructor
 */
VectorSmoothing::VectorSmoothing() {
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "Label of the estimate to generate penalty on", "");
  parameters_.Bind<bool>(PARAM_LOG_SCALE, &log_scale_, "Log scale", "", false);
  parameters_.Bind<double>(PARAM_MULTIPLIER, &multiplier_, "Multiplier for the penalty amount", "", 1);
  parameters_.Bind<unsigned>(PARAM_STEP_SIZE, &step_size_, "The element index to use", "", 3);
}

/**
 * Validate our parameters
 */
void VectorSmoothing::DoValidate() {}

/**
 * Build our parameters
 */
void VectorSmoothing::DoBuild() {
  string type       = "";
  string label      = "";
  string parameter  = "";
  string index      = "";

  /**
   * Explode the parameter string sive o we can get the estimable
   * name (parameter) and the index
   */
  if (parameter_ == "") {
    parameters().Add(PARAM_PARAMETER, label_, parameters_.Get(PARAM_LABEL)->file_name(), parameters_.Get(PARAM_LABEL)->line_number());
    parameter_ = label_;
  }

  objects::ExplodeString(parameter_, type, label, parameter, index);
  if (type == "" || label == "" || parameter == "") {
    LOG_ERROR(parameters_.location(PARAM_PARAMETER) << ": parameter " << parameter_
        << " is not in the correct format. Correct format is object_type[label].estimable(array index)");
  }

  base::ObjectPtr target = objects::FindObject(parameter_);
  if (!target)
    LOG_ERROR(parameters_.location(PARAM_PARAMETER) << " " << parameter_ << " is not a valid estimable in the system");


  Estimable::Type estimable_type = target->GetEstimableType(parameter);
  switch(estimable_type) {
    case Estimable::kInvalid:
      LOG_CODE_ERROR("Invalid estimable type: " << parameter_);
      break;
    case Estimable::kVector:
      estimable_vector_ = target->GetEstimableVector(parameter);
      break;
    case Estimable::kUnsignedMap:
      estimable_map_ = target->GetEstimableUMap(parameter);
      break;
    default:
      LOG_ERROR("The estimable you have provided for use in a projection: " << parameter_ << " is not a type that is supported for projection modification");
      break;
  }
}

/**
 * Get the score for this penalty
 *
 * @return Penalty score
 */
Double VectorSmoothing::GetScore() {
  vector<Double> values;
  if (estimable_vector_ != 0)
    values.assign((*estimable_vector_).begin(), (*estimable_vector_).end());
  else if (estimable_map_ != 0) {
    for (auto iter : (*estimable_map_))
      values.push_back(iter.second);
  } else
    LOG_CODE_ERROR("(estimable_map_ != 0) && (estimable_map_ != 0)");

  Double score;

  if (log_scale_) {
    for (Double& value : values)
      value = log(value);
  }

  Double first_value = values[0];
  for (unsigned i = step_size_; i < values.size(); i += step_size_) {
    score += abs(values[i] - first_value) * abs(values[i] - first_value);
    first_value = values[i];
  }

  return score * multiplier_;
}

} /* namespace penalties */
} /* namespace isam */
