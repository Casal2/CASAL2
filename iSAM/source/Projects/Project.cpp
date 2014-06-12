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
namespace isam {

/**
 * Default constructor
 */
Project::Project() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to recalculate the values", "", true);
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "Parameter to project", "");
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
    LOG_ERROR(parameters_.location(PARAM_PARAMETER) << ": parameter " << parameter_ << " is not a valid estimable in the system");

  Estimable::Type estimable_type = target->GetEstimableType(parameter);
  switch(estimable_type) {
    case Estimable::kInvalid:
      LOG_CODE_ERROR("Invalid estimable type: " << parameter_);
      break;
    case Estimable::kSingle:
      DoUpdateFunc_ = &Project::SetSingleValue;
      break;
    case Estimable::kVector:
      DoUpdateFunc_ = &Project::SetVectorValue;
      break;
    case Estimable::kMap:
      DoUpdateFunc_ = &Project::SetMapValue;
      break;
  }

//  target_ = target->GetEstimable(parameter);
}

} /* namespace isam */
