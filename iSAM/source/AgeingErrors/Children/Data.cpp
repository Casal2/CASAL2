/**
 * @file Data.cpp
 * @author C Marsh
 * @github https://github.com/Zaita
 * @date 01/09/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Data.h"

// namespaces
namespace niwa {
namespace ageingerrors {

Data::Data() {
  parameters_.BindTable(PARAM_DATA, data_table_, "", "");
  parameters_.Bind<string>(PARAM_EXTERNAL_GAPS, &external_gaps_, "", "", PARAM_MEAN)->set_allowed_values({PARAM_MEAN, PARAM_NEAREST_NEIGHBOUR});
  parameters_.Bind<string>(PARAM_INTERNAL_GAPS, &internal_gaps_, "", "", PARAM_MEAN)->set_allowed_values({PARAM_MEAN, PARAM_NEAREST_NEIGHBOUR, PARAM_INTERPOLATE});
}

/**
 *  Validate any paramters
*/

void Data::DoValidate() {

}

void Data::DoBuild() {
  DoReset();
}

void Data::DoReset() {

}


} /* namespace ageingerrors */
} /* namespace niwa */
