/**
 * @file Manager.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Oct 26, 2016
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "Manager.h"

#include "../Likelihoods/Factory.h"
#include "../Model/Model.h"

// namespaces
namespace niwa {
namespace likelihoods {

/**
 * class definition
 */
Manager::Manager() {}

/**
 * This method returns an existing likelihood or nullptr
 *
 * @param label potential likelihood label
 * @return pointer to Likelihood
 */
Likelihood* Manager::GetLikelihood(const string& label) {
  for (auto likelihood : objects_) {
    if (likelihood->label() == label)
      return likelihood;
  }

  return nullptr;
}
/**
 * This method returns the existing likelihood or creates and returns the likelihood
 *
 * @param model
 * @param observation_label
 * @param label potential likelihood label
 * @return pointer to Likelihood
 */
Likelihood* Manager::GetOrCreateLikelihood(shared_ptr<Model> model, const string& observation_label, const string& label) {
  LOG_FINEST() << observation_label << " + " << label;
  Likelihood* labelled = nullptr;
  Likelihood* factory  = nullptr;

  for (auto likelihood : objects_) {
    LOG_FINEST() << likelihood->label();
    if (likelihood->label() == label) {
      LOG_FINEST() << "Matched: " << label;
      labelled = likelihood;
      break;
    }
  }

  factory = likelihoods::Factory::Create(model, PARAM_LIKELIHOOD, label);
  if (labelled != nullptr && factory != nullptr) {
    LOG_ERROR() << labelled->location() << " likelihood " << label << " has the label that matches a type of likelihood.";
  }

  if (labelled == nullptr && factory == nullptr)
    return nullptr;

  if (labelled != nullptr) {
    LOG_FINE() << "Returning likelihood: " << labelled->label();
    return labelled;
  }

  factory->set_type(label);
  factory->set_label(observation_label + "." + label);
  return factory;
}

} /* namespace likelihoods */
} /* namespace niwa */
