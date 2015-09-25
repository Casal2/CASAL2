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
namespace niwa {
namespace penalties {

/**
 * Default constructor
 */
VectorSmoothing::VectorSmoothing() {
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "Label of the estimate to generate penalty on", "");
  parameters_.Bind<bool>(PARAM_LOG_SCALE, &log_scale_, "Log scale", "", false);
  parameters_.Bind<Double>(PARAM_MULTIPLIER, &multiplier_, "Multiplier for the penalty amount", "", 1);
  parameters_.Bind<unsigned>(PARAM_LOWER_BOUND, &lower_, "First element to apply the penalty to in the vector", "", 0u);
  parameters_.Bind<unsigned>(PARAM_UPPER_BOUND, &upper_, "Last element to apply the penalty to in the vector", "", 0u);
  parameters_.Bind<unsigned>(PARAM_R, &r_, "Penalty applied to rth differences", "", 2u);
}

/**
 * Validate our parameters
 */
void VectorSmoothing::DoValidate() {
}

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
    LOG_ERROR_P(PARAM_PARAMETER) << ": parameter " << parameter_
        << " is not in the correct format. Correct format is object_type[label].estimable(array index)";
  }

  base::Object* target = objects::FindObject(parameter_);
  if (!target)
    LOG_ERROR_P(PARAM_PARAMETER) << " " << parameter_ << " is not a valid estimable in the system";


  Estimable::Type estimable_type = target->GetEstimableType(parameter);
  switch(estimable_type) {
    case Estimable::kInvalid:
      LOG_CODE_ERROR() << "Invalid estimable type: " << parameter_;
      break;
    case Estimable::kVector:
      estimable_vector_ = target->GetEstimableVector(parameter);
      break;
    case Estimable::kUnsignedMap:
      estimable_map_ = target->GetEstimableUMap(parameter);
      break;
    default:
      LOG_ERROR() << "The estimable you have provided for use in a projection: " << parameter_ << " is not a type that is supported for projection modification";
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
    LOG_CODE_ERROR() << "(estimable_map_ != 0) && (estimable_map_ != 0)";

  if(upper_ == lower_ && upper_ == 0u) {
    upper_ = values.size();
    lower_ = 1;
  }

  if(upper_ == lower_)
    LOG_FATAL_P(PARAM_UPPER_BOUND) << "Lower and upper bound cannot be equal";
  if (upper_ > values.size())
    LOG_FATAL_P(PARAM_UPPER_BOUND) << "The last element must not be greater than size of vector";
  if (lower_ < 1)
    LOG_FATAL_P(PARAM_LOWER_BOUND) << "The first element must not be less than 1";

  if (r_ >= (upper_ - lower_))
  LOG_FATAL_P(PARAM_R) << PARAM_R << " R cannot be greater than or equal to size of vector - 1";

  Double score = 0.0;
  if (log_scale_) {
    for (Double& value : values)
      value = log(value);
  }
  for (unsigned i = 1; i <= r_; ++i) {
    for(unsigned j = (lower_ - 1); j <= ((upper_ - 1) - i); ++j) {
      values[j] = values[j + 1] - values[j];
    }
    values[(upper_ - 1) - i + 1] = 0;
  }

  for (unsigned k = (lower_ - 1); k <= (upper_ - 1); ++k)
    score += values[k] * values[k];
  return score * multiplier_;
}

} /* namespace penalties */
} /* namespace niwa */
