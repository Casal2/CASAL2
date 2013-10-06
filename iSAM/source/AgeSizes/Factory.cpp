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

#include "AgeSizes/Manager.h"
#include "AgeSizes/AgeSize.h"
#include "AgeSizes/Children/None.h"
#include "AgeSizes/Children/Schnute.h"
#include "AgeSizes/Children/VonBertalanffy.h"

// namespaces
namespace isam {
namespace agesizes {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
AgeSizePtr Factory::Create(const string& object_type, const string& sub_type) {
  AgeSizePtr result;

  if (object_type == PARAM_AGE_SIZE || object_type == PARAM_AGE_SIZES) {
    if (sub_type == PARAM_NONE)
      result = AgeSizePtr(new None());
    else if (sub_type == PARAM_SCHNUTE)
      result = AgeSizePtr(new Schnute());
    else if (sub_type == PARAM_VON_BERTALANFFY)
      result = AgeSizePtr(new VonBertalanffy());

    if (result)
      agesizes::Manager::Instance().AddObject(result);
  }

  return result;
}

} /* namespace agesizes */
} /* namespace isam */
