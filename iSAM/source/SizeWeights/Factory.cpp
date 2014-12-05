/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "SizeWeights/Manager.h"
#include "SizeWeights/Children/Basic.h"
#include "SizeWeights/Children/None.h"

// namespaces
namespace niwa {
namespace sizeweights {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
SizeWeightPtr Factory::Create(const string& object_type, const string& sub_type) {
  SizeWeightPtr result;

  if (object_type == PARAM_SIZE_WEIGHT || object_type == PARAM_SIZE_WEIGHTS) {
    if (sub_type == PARAM_NONE)
      result = SizeWeightPtr(new None());
    else if (sub_type == PARAM_BASIC)
      result = SizeWeightPtr(new Basic());

    if (result)
      sizeweights::Manager::Instance().AddObject(result);
  }

  return result;
}

} /* namespace sizeweights */
} /* namespace niwa */
