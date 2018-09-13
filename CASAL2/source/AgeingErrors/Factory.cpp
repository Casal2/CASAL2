/**
 * @file Factory.cpp
 * @author  C. Marsh
 * @date 31/08/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "Model/Model.h"
#include "Model/Managers.h"
#include "AgeingErrors/Manager.h"
#include "AgeingErrors/AgeingError.h"
#include "AgeingErrors/Age/Data.h"
#include "AgeingErrors/Age/Normal.h"
#include "AgeingErrors/Age/OffByOne.h"
#include "AgeingErrors/Age/None.h"

// namespaces
namespace niwa {
namespace ageingerrors {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
AgeingError* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  AgeingError* result = nullptr;

  if (object_type == PARAM_AGEING_ERROR || object_type == PARAM_AGEING_ERRORS) {
    if (sub_type == PARAM_DATA)
      result = new Data(model);
    else if (sub_type == PARAM_NORMAL)
      result = new Normal(model);
    else if (sub_type == PARAM_OFF_BY_ONE)
      result = new OffByOne(model);
    else if (sub_type == PARAM_NONE)
      result = new None(model);

    if (result)
      model->managers().ageing_error()->AddObject(result);
  }

  return result;
}

} /* namespace AgeingErrors */
} /* namespace niwa */
