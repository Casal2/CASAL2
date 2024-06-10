/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "Factory.h"

#include "../Model/Managers.h"
#include "../Model/Model.h"
#include "../Projects/Common/Constant.h"
#include "../Projects/Common/EmpiricalSampling.h"
#include "../Projects/Common/LogNormal.h"
#include "../Projects/Common/LogNormalEmpirical.h"
#include "../Projects/Common/MultipleConstants.h"
#include "../Projects/Common/UserDefined.h"
#include "../Projects/Manager.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Create the instance of the object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g., age_size, process)
 * @param sub_type The child type of the object to create (e.g., ageing, schnute)
 * @return shared_ptr to the object
 */
Project* Factory::Create(shared_ptr<Model> model, const string& object_type, const string& sub_type) {
  Project* result = nullptr;

  if (object_type == PARAM_PROJECTS || object_type == PARAM_PROJECT) {
    if (sub_type == PARAM_CONSTANT)
      result = new Constant(model);
    else if (sub_type == PARAM_LOGNORMAL)
      result = new LogNormal(model);
    else if (sub_type == PARAM_LOGNORMAL_EMPIRICAL)
      result = new LogNormalEmpirical(model);
    else if (sub_type == PARAM_EMPIRICAL_SAMPLING)
      result = new EmpiricalSampling(model);
    else if (sub_type == PARAM_MULTIPLE_VALUES)
      result = new MultipleConstants(model);
    //else if (sub_type == PARAM_USER_DEFINED)
    //  result = new UserDefined(model);
    if (result)
      model->managers()->project()->AddObject(result);
  }

  return result;
}

} /* namespace projects */
} /* namespace niwa */
