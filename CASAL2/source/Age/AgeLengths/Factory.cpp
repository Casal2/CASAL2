/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 7/10/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"
#include "Age/AgeLengths/Manager.h"
#include "Age/AgeLengths/AgeLength.h"
#include "Age/AgeLengths/Children/Data.h"
#include "Age/AgeLengths/Children/None.h"
#include "Age/AgeLengths/Children/Schnute.h"
#include "Age/AgeLengths/Children/VonBertalanffy.h"

// namespaces
namespace niwa {
namespace agelengths {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
AgeLength* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  AgeLength* result = nullptr;

  if (object_type == PARAM_AGE_LENGTH || object_type == PARAM_AGE_LENGTHS) {
    if (sub_type == PARAM_DATA)
      result = new Data(model);
    else if (sub_type == PARAM_NONE)
      result = new None(model);
    else if (sub_type == PARAM_SCHNUTE)
      result = new Schnute(model);
    else if (sub_type == PARAM_VON_BERTALANFFY)
      result = new VonBertalanffy(model);

    if (result)
      model->managers().age_length()->AddObject(result);
  }

  return result;
}

} /* namespace agelengths */
} /* namespace niwa */
