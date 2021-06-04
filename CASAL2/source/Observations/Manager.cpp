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
namespace niwa {
namespace observations {

/**
 * Default constructor
 */
Manager::Manager() {}

/**
 * Destructor
 */
Manager::~Manager() noexcept(true) {}

/**
 * This method returns a pointer to the observation
 *
 * @param label The label of the observation
 * @return a pointer to the Observation
 */
Observation* Manager::GetObservation(const string& label) {
  for (auto observation : objects_) {
    if (observation->label() == label)
      return observation;
  }

  return nullptr;
}

/**
 * PreExecute our observations that have matching year and time step labels.
 * This is usually used to build caches of something before the time step
 * has been executed
 *
 * @param year The current year the model is in
 * @param time_step The time step that is about to be executed
 */
// void Manager::PreExecute(unsigned year, const string& time_step) {
//  for (ObservationPtr observation : objects_) {
//    if (observation->HasYear(year) && observation->time_step() == time_step)
//      observation->PreExecute();
//  }
//}

/**
 * Execute our observations that have matching year and time step labels
 *
 * @param year The current year the model is in
 * @param time_step The time step that just finished execution
 */
// void Manager::Execute(unsigned year, const string& time_step) {
//  LOG_TRACE();
//  for (ObservationPtr observation : objects_) {
//    if (observation->HasYear(year) && observation->time_step() == time_step)
//      observation->Execute();
//  }
//}

/**
 * This method calculates the score for all objects
 */
void Manager::CalculateScores() {
  for (auto observation : objects_) {
    observation->CalculateScore();
  }
}

} /* namespace observations */
} /* namespace niwa */
