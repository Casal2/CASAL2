/**
 * @file VectorAverage.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "VectorAverage.h"

#include "Model/Model.h"
#include "Model/Objects.h"

// namespaces
namespace niwa {
namespace additionalpriors {

/**
 * Default constructor
 */
VectorAverage::VectorAverage(Model* model) : AdditionalPrior(model) {
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "Name of the parameter to generate additional prior on", "");
  parameters_.Bind<string>(PARAM_METHOD, &method_, "What calculation method to use, either k, l, or m", "", PARAM_K);
  parameters_.Bind<Double>(PARAM_K, &k_, "K Value to use in the calculation", "");
  parameters_.Bind<Double>(PARAM_MULTIPLIER, &multiplier_, "Multiplier for the penalty amount", "", 1);
}

/**
 * Validate our parameters
 */
void VectorAverage::DoValidate() { }

/**
 * Build relationships between this object and other objects
 */
void VectorAverage::DoBuild() {
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

  model_->objects().ExplodeString(parameter_, type, label, parameter, index);
  if (type == "" || label == "" || parameter == "") {
    LOG_ERROR_P(PARAM_PARAMETER) << ": parameter " << parameter_
        << " is not in the correct format. Correct format is object_type[label].estimable(array index)";
  }

  string error = "";
  base::Object* target = model_->objects().FindObject(parameter_, error);
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
      LOG_ERROR() << "The estimable you have provided for use in a additional priors: " << parameter_ << " is not a type that is supported for additional priors";
      break;
  }
}

/**
 * Get the score for this penalty
 *
 * @return Penalty score
 */
Double VectorAverage::GetScore() {
  vector<Double> values;
  if (estimable_vector_ != 0)
    values.assign((*estimable_vector_).begin(), (*estimable_vector_).end());
  else if (estimable_map_ != 0) {
    for (auto iter : (*estimable_map_))
      values.push_back(iter.second);
  } else
    LOG_CODE_ERROR() << "(estimable_map_ != 0) && (estimable_map_ != 0)";

  Double score = 0.0;

  if (method_ == PARAM_K) {
    for (Double value : values)
      score += value;
    score = score / values.size();

    score = (score - k_) * (score - k_);

  } else if (method_ == PARAM_L) {
    for (Double value : values)
      score += log(value);
    score = score / values.size();

    score = (score - k_) * (score - k_);

  } else if (method_ == PARAM_M) {
    for (Double value : values)
      score += value;
    score = log(score / values.size());

    score = (score - k_) * (score - k_);

  } else
    LOG_CODE_ERROR() << "Unknown Method: " << method_;

  return score * multiplier_;
}

} /* namespace additionalpriors */
} /* namespace niwa */
