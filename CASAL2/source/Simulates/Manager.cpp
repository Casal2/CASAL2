/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/�2014 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

// namespaces
namespace niwa {
namespace simulates {

/**
 * Default constructor
 */
Manager::Manager() {}

/**
 * Destructor
 */
Manager::~Manager() noexcept(true) {}

/**
 * Update the objects for a specific year
 *
 * @param year The year
 */
void Manager::Update(unsigned current_year) {
  //  LOG_TRACE();
  //  for (ProjectPtr project : objects_)
  //    project->Update(current_year);
}

} /* namespace simulates */
} /* namespace niwa */
