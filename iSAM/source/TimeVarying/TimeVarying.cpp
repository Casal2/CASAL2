/**
 * @file TimeVarying.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 27/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "TimeVarying.h"

#include "ObjectsFinder/ObjectsFinder.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {

/**
 * Default constructor
 */
TimeVarying::TimeVarying() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to recalculate the values", "");
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "Parameter to project", "");

  model_ = Model::Instance();
  original_value_ = 0;
}

/**
 *
 */
void TimeVarying::Validate() {
  parameters_.Populate();
  DoValidate();
}

/**
 *
 */
void TimeVarying::Build() {
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

  base::ObjectPtr target = objects::FindObject(parameter_);
  if (!target)
    LOG_ERROR_P(PARAM_PARAMETER) << " " << parameter_ << " is not a valid estimable in the system";

  Estimable::Type estimable_type = target->GetEstimableType(parameter);
  switch(estimable_type) {
    case Estimable::kInvalid:
      LOG_CODE_ERROR() << "Invalid estimable type: " << parameter_;
      break;
    case Estimable::kSingle:
      update_function_ = &TimeVarying::set_single_value;
      estimable_    = target->GetEstimable(parameter);
      original_value_ = *estimable_;
      break;
    case Estimable::kVector:
      update_function_ = &TimeVarying::set_vector_value;
      estimable_vector_ = target->GetEstimableVector(parameter);
      break;
    case Estimable::kUnsignedMap:
      update_function_ = &TimeVarying::set_map_value;
      estimable_map_ = target->GetEstimableUMap(parameter);
      break;
    default:
      LOG_ERROR() << "The estimable you have provided for use in a projection: " << parameter_ << " is not a type that is supported for projection modification";
      break;
  }
  DoBuild();
}

/**
 *
 */
void TimeVarying::Update(unsigned current_year) {
  LOG_TRACE();
  if (update_function_ == 0)
    LOG_CODE_ERROR() << "DoUpdateFunc_ == 0";

  if (years_.size() > 0 && std::find(years_.begin(), years_.end(), current_year) == years_.end())
    RestoreOriginalValue();
  else
    DoUpdate();
}

/**
 *
 */
void TimeVarying::RestoreOriginalValue() {
  LOG_TRACE();
  LOG_FINE() << "Setting original value to: " << original_value_;
  (this->*update_function_)(original_value_);
}

/**
 *
 */
void TimeVarying::set_single_value(Double value) {
  *estimable_ = value;
}

/**
 *
 */
void TimeVarying::set_vector_value(Double value) {
  estimable_vector_->push_back(value);
}

/**
 *
 */
void TimeVarying::set_map_value(Double value) {
  (*estimable_map_)[model_->current_year()] = value;
}

} /* namespace niwa */
