/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers

#include "Factory.h"

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"
#include "Common/Reports/Manager.h"
#include "Age/Reports/Children/AgeingErrorMatrix.h"
#include "Age/Reports/Children/AgeLength.h"
#include "Age/Reports/Children/InitialisationPartitionMeanWeight.h"
#include "Age/Reports/Children/PartitionMeanWeight.h"
#include "Age/Reports/Children/PartitionBiomass.h"
#include "Age/Reports/Children/Partition.h"

// Namespaces
namespace niwa {
namespace age {
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
    if (sub_type == PARAM_AGEING_ERROR)
      result = new AgeingErrorMatrix(model);
    if (sub_type == PARAM_AGE_LENGTH)
      result = new AgeLength(model);
    else if (sub_type == PARAM_PARTITION_BIOMASS)
      result = new PartitionBiomass(model);
    else if (sub_type == PARAM_PARTITION_MEAN_WEIGHT)
      result = new PartitionMeanWeight(model);
    else if (sub_type == PARAM_INITIALISATION_PARTITION_MEAN_WEIGHT)
      result = new InitialisationPartitionMeanWeight(model);
    else if (sub_type == PARAM_PARTITION)
      result = new Partition(model);
  }

  return result;
}

} /* namespace reports */
} /* namespace age */
} /* namespace niwa */
