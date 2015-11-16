/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 6/06/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "Model/Model.h"
#include "Model/Managers.h"
#include "DerivedQuantities/Manager.h"
#include "DerivedQuantities/Children/Abundance.h"
#include "DerivedQuantities/Children/Biomass.h"

// namespaces
namespace niwa {
namespace derivedquantities {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
DerivedQuantity* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  DerivedQuantity* result = nullptr;

  if (object_type == PARAM_DERIVED_QUANTITY || object_type == PARAM_DERIVED_QUANTITIES) {
    if (sub_type == PARAM_ABUNDANCE)
      result = new Abundance(model);
    else if (sub_type == PARAM_BIOMASS)
      result = new Biomass(model);

    if (result)
      model->managers().derived_quantity()->AddObject(result);
  }

  return result;
}


} /* namespace derivedquantities */
} /* namespace niwa */
