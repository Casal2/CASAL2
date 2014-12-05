/**
 * @file Project.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Project.h"

#include "ObjectsFinder/ObjectsFinder.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {

/**
 * Default constructor
 */
Project::Project() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to recalculate the values", "", true);
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "Parameter to project", "");

  model_ = Model::Instance();
  original_value_ = 0;
}

/**
 *
 */
void Project::Validate() {
  parameters_.Populate();
  DoValidate();
}

/**
 *
 */
void Project::Build() {
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
    case Estimable::kSingle:
      DoUpdateFunc_ = &Project::SetSingleValue;
      estimable_    = target->GetEstimable(parameter);
      original_value_ = *estimable_;
      break;
    case Estimable::kVector:
      DoUpdateFunc_ = &Project::SetVectorValue;
      estimable_vector_ = target->GetEstimableVector(parameter);
      break;
    case Estimable::kUnsignedMap:
      DoUpdateFunc_ = &Project::SetMapValue;
      estimable_map_ = target->GetEstimableUMap(parameter);
      break;
    default:
      LOG_ERROR("The estimable you have provided for use in a projection: " << parameter_ << " is not a type that is supported for projection modification");
      break;
  }
  DoBuild();
}

/**
 *
 */
void Project::Update(unsigned current_year) {
  LOG_TRACE();
  if (DoUpdateFunc_ == 0)
    LOG_CODE_ERROR("DoUpdateFunc_ == 0");

  if (years_.size() > 0 && std::find(years_.begin(), years_.end(), current_year) == years_.end())
    RestoreOriginalValue();
  else
    DoUpdate();
}

/**
 *
 */
void Project::RestoreOriginalValue() {
  LOG_TRACE();
  LOG_INFO("Setting original value to: " << original_value_);
  (this->*DoUpdateFunc_)(original_value_);
}

/**
 *
 */
void Project::SetSingleValue(Double value) {
  *estimable_ = value;
}

/**
 *
 */
void Project::SetVectorValue(Double value) {
  estimable_vector_->push_back(value);
}

/**
 *
 */
void Project::SetMapValue(Double value) {
  (*estimable_map_)[model_->current_year()] = value;
}

} /* namespace niwa */



















