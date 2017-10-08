/**
 * @file Factory.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 1/08/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */

// Headers

#include "Factory.h"

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"
#include "Common/Reports/Manager.h"
#include "Length/Reports/Children/InitialisationPartitionMeanWeight.h"
#include "Length/Reports/Children/PartitionMeanWeight.h"
#include "Length/Reports/Children/PartitionBiomass.h"
#include "Length/Reports/Children/Partition.h"



// Namespaces
namespace niwa {
namespace length {
namespace reports {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
Report* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  Report* result = nullptr;

  if (object_type == PARAM_REPORT) {
    if (sub_type == PARAM_INITIALISATION_PARTITION_MEAN_WEIGHT)
      result = new InitialisationPartitionMeanWeight(model);
    else if (sub_type == PARAM_PARTITION_MEAN_WEIGHT)
      result = new PartitionMeanWeight(model);
    else if (sub_type == PARAM_PARTITION_BIOMASS)
      result = new PartitionBiomass(model);
    else if (sub_type == PARAM_PARTITION)
      result = new Partition(model);
  }

  return result;
}

} /* namespace reports */
} /* namespace length */
} /* namespace niwa */
