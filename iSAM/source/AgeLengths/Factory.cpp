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

#include "AgeLengths/Manager.h"
#include "AgeLengths/AgeLength.h"
#include "AgeLengths/Children/Data.h"
#include "AgeLengths/Children/None.h"
#include "AgeLengths/Children/Schnute.h"
#include "AgeLengths/Children/VonBertalanffy.h"

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
AgeLengthPtr Factory::Create(const string& object_type, const string& sub_type) {
  AgeLengthPtr result;

  if (object_type == PARAM_AGE_LENGTH || object_type == PARAM_AGE_LENGTHS) {
    if (sub_type == PARAM_DATA)
      result = AgeLengthPtr(new Data());
    else if (sub_type == PARAM_NONE)
      result = AgeLengthPtr(new None());
    else if (sub_type == PARAM_SCHNUTE)
      result = AgeLengthPtr(new Schnute());
    else if (sub_type == PARAM_VON_BERTALANFFY)
      result = AgeLengthPtr(new VonBertalanffy());

    if (result)
      agelengths::Manager::Instance().AddObject(result);
  }

  return result;
}

} /* namespace agelengths */
} /* namespace niwa */
