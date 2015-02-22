/**
 * @file Category.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 18/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 */

// headers
#include "Category.h"

// namespaces
namespace niwa {
namespace partition {

/**
 * This method will populate the length data on the partition category object.
 * This is required to be called by any object that wants to update the length
 * data from the age data.
 */
void Category::UpdateLengthData() {

}

/**
 * This method will populate the age data from the length data. This is required
 * to transfer any changes in the length partition back to the age partition.
 */
void Category::UpdateAgeData() {

}

/**
 * This method will update the weight data with the number of fish and weight
 * per fish for use.
 */
void Category::UpdateWeightData() {

}

/**
 *
 */
void Category::UpdateSizeData() {

}

} /* namespace partitions */
} /* namespace niwa */
