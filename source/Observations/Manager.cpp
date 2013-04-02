/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Manager.h"

// Namespaces
namespace isam {
namespace observations {

/**
 * Default Constructor
 */
Manager::Manager() {
}

/**
 * Destructor
 */
Manager::~Manager() noexcept(true) {
}

/**
 * PreExecute our observations that have matching year and time step labels.
 * This is usually used to build caches of something before the time step
 * has been executed
 *
 * @param year The current year the model is in
 * @param time_step The time step that is about to be executed
 */
void Manager::PreExecute(unsigned year, const string& time_step) {
  for (ObservationPtr observation : objects_) {
    if (observation->year() == year && observation->time_step() == time_step)
      observation->PreExecute();
  }
}

/**
 * Execute our observations that have matching year and time step labels
 *
 * @param year The current year the model is in
 * @param time_step The time step that just finished execution
 */
void Manager::Execute(unsigned year, const string& time_step) {
  for (ObservationPtr observation : objects_) {
    if (observation->year() == year && observation->time_step() == time_step)
      observation->Execute();
  }
}

} /* namespace observations */
} /* namespace isam */
