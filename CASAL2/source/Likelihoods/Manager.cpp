/**
 * @file Manager.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Oct 26, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

#include "Likelihoods/Factory.h"

// namespaces
namespace niwa {
namespace likelihoods {

/**
 * class definition
 */
Manager::Manager() {
}

/**
 *
 */
Likelihood* Manager::GetOrCreateLikelihood(const string& label) {
  Likelihood* labelled = nullptr;
  Likelihood* factory  = nullptr;

  for (auto likelihood : objects_) {
    if (likelihood->label() == label) {
      labelled = likelihood;
      break;
    }
  }

  factory = likelihoods::Factory::Create(label);
  if (labelled != nullptr && factory != nullptr) {
    LOG_ERROR() << labelled->location() << " likelihood " << label << " has the label that matches a type of likelihood. This is not allowed";
  }

  if (labelled != nullptr)
    return labelled;

  objects_.push_back(factory);
  return factory;
}

} /* namespace likelihoods */
} /* namespace niwa */
