/**
 * @file EmpiricalSampling.cpp
 * @author  Craig Marsh
 * @date 05/02/2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */

// headers
#include "EmpiricalSampling.h"


// namespaces
namespace niwa {
namespace projects {

/**
 * Default constructor
 */
EmpiricalSampling::EmpiricalSampling(Model* model) : Project(model) {
  parameters_.Bind<unsigned>(PARAM_START_YEAR, &start_year_, "Start year of sampling", "");
  parameters_.Bind<unsigned>(PARAM_FINAL_YEAR, &final_year_, "Last year of sampling", "");
}

/**
 * Validate
 */
void EmpiricalSampling::DoValidate() {
  if (final_year_ <= start_year_)
    LOG_ERROR() << PARAM_FINAL_YEAR << " must be larger than " << PARAM_START_YEAR;

}

/**
 * Build
 */
void EmpiricalSampling::DoBuild() {
  // validate that the parameter exists over this year range then
  // Build up a map of all the years and the parameter value for each year

}

/**
 * Reset
 */
void EmpiricalSampling::DoReset() { }

/**
 *  Update our parameter with a random resample of the parameter between start_year_ and final_year_
 */
void EmpiricalSampling::DoUpdate() {


}

} /* namespace projects */
} /* namespace niwa */
