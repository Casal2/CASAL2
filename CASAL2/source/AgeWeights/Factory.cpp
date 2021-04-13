/**
 * @file Factory.cpp
 * @author  C.Marsh
 * @date 16/11/2017
 * @section LICENSE
 *
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "../Model/Model.h"
#include "../Model/Managers.h"
#include "../AgeWeights/Manager.h"
#include "../AgeWeights/AgeWeight.h"
#include "../AgeWeights/Age/Data.h"
#include "../AgeWeights/Age/None.h"

// namespaces
namespace niwa {
namespace ageweights {

/**
 * Create the instance of the object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g., age_size, process)
 * @param sub_type The child type of the object to create (e.g., ageing, schnute)
 * @return shared_ptr to the object created
 */
AgeWeight* Factory::Create(shared_ptr<Model> model, const string& object_type, const string& sub_type) {
  AgeWeight* result = nullptr;

  if (object_type == PARAM_AGE_WEIGHT) {
    if (sub_type == PARAM_DATA)
      result = new Data(model);
    else if (sub_type == PARAM_DATA)
      result = new None(model);


    if (result)
      model->managers()->age_weight()->AddObject(result);

  }
  return result;
}

} /* namespace ageweights */
} /* namespace niwa */
