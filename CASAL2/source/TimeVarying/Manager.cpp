/**
 * @file Manager.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 27/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

#include "Logging/Logging.h"

// namespaces
namespace niwa {
namespace timevarying {

/**
 * Default constructor
 */
Manager::Manager() {

}

/**
 * Update all time-varying objects for a specific year
 *
 * @param current_year The year in which to update all time-varying objects
 */
void Manager::Update(unsigned current_year) {
  LOG_TRACE();
  for (auto time_varying : objects_) {
    time_varying->Update(current_year);
    LOG_FINEST() << "updating time_varying class " << time_varying->label();
  }
}

/**
 * Does this label reference a time-varying label
 *
 * @param label The label of the time-varying object
 * @return If true, then this is a time-varying object
 */
bool Manager::IsTimeVarying(const string& label) {
  for (auto time_varying : objects_) {
    if (time_varying->label() == label) {
      return true;
    }
  }

  return false;
}

/**
 * Does this label reference a time-varying target
 *
 * @param label The label of the time-varying target object
 * @return If true, then this is a time-varying target object
 */
bool Manager::IsTimeVaryingTarget(const string& label) {
  for (auto time_varying : objects_) {
    if (utilities::ToLowercase(time_varying->get_target_parameter_label()) == utilities::ToLowercase(label)) {
      return true;
    }
  }

  return false;
}

/**
 * Get the pointer for the time-varying label
 *
 * @param label The label of the time-varying object
 * @return time-varying smart_ptr
 */
TimeVarying* Manager::GetTimeVarying(const string& label) {
  for (auto time_varying : objects_) {
    if (time_varying->label() == label) {
      return time_varying;
    }
  }

  return nullptr;
}

/**
 * Count how many time-varying parameters there are
 * and return the count
 *
 * @return The number of time-varying parameters
 */
unsigned Manager::GetTimeVaryingCount() {
  unsigned count = 0;

  for (auto time_varying : objects_) {
    if (time_varying->label() != "")
      count++;
  }

  return count;
}

} /* namespace processes */
} /* namespace niwa */
