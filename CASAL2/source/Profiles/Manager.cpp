/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/03/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

#include "Verification/Verification.h"

// namespaces
namespace niwa {
namespace profiles {

/**
 * Default constructor
 */
Manager::Manager() {}
/**
 * Return the profile with the name passed in as a parameter.
 * If no process is found then an empty pointer will
 * be returned.
 *
 * @return A pointer to the profile or empty pointer
 */
Profile* Manager::GetProfile() {
  if (objects_.size() > 1)
    LOG_CODE_ERROR() << "Found " << objects_.size() << " @profile blocks. Casal2 can only be run with one profile block.";
  if (objects_.size() > 0) {
    return objects_[0];
  }
  return nullptr;
}

/**
 * @brief Verify our profiles
 *
 * @param model
 */
void Manager::Verify(shared_ptr<Model> model) {
  LOG_FINE() << "Verify profile";
  verification::DoVerification(model);
  for (auto profile : objects_) profile->Verify(model);
}
} /* namespace profiles */
} /* namespace niwa */
