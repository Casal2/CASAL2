/**
 * @file Factory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 8/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "BaseClasses/Object.h"
#include "AdditionalPriors/Factory.h"
#include "AgeingErrors/Factory.h"
#include "AgeLengths/Factory.h"
#include "Asserts/Factory.h"
#include "Catchabilities/Factory.h"
#include "Categories/Categories.h"
#include "DerivedQuantities/Factory.h"
#include "Estimates/Creators/Factory.h"
#include "InitialisationPhases/Factory.h"
#include "Likelihoods/Factory.h"
#include "MCMCs/Factory.h"
#include "Minimisers/Factory.h"
#include "Model/Model.h"
#include "Observations/Factory.h"
#include "Penalties/Factory.h"
#include "Processes/Factory.h"
#include "Profiles/Factory.h"
#include "Projects/Factory.h"
#include "Reports/Factory.h"
#include "Selectivities/Factory.h"
#include "LengthWeights/Factory.h"
#include "TimeSteps/Factory.h"
#include "TimeVarying/Factory.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {

/**
 * Default constructor
 */
Factory::Factory(Model* model) : model_(model) { }

/**
 * Create an ObjectPtr for a specific class type in our system. This method
 * will check the object_type and find the appropriate child factory to call
 * so the object can be created properly.
 *
 * This design was picked to simplify how the factories work because while having
 * a single super-factory would be the simplest solution we need objects to be created
 * in their child form (e.g AgeSizePtr) to be used properly in the code without
 * having to do dynamic casting (slow).
 *
 * @param object_type The type of object to create (e.g process, selectivity)
 * @param sub_type The specialisation/sub_type of the object to create
 * @return A shared_ptr to the object we've created
 */
base::Object* Factory::CreateObject(string& object_type, string& sub_type) {
  object_type = utilities::ToLowercase(object_type);
  sub_type    = utilities::ToLowercase(sub_type);

  if (object_type == PARAM_ADDITIONAL_PRIOR)
    return additionalpriors::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_AGEING_ERROR || object_type == PARAM_AGEING_ERRORS)
    return ageingerrors::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_AGE_LENGTH || object_type == PARAM_AGE_LENGTHS)
    return agelengths::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_ASSERT)
    return asserts::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_CATCHABILITY)
    return catchabilities::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_CATEGORIES)
    return model_->categories();
  else if (object_type == PARAM_DERIVED_QUANTITY || object_type == PARAM_DERIVED_QUANTITIES)
    return derivedquantities::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_ESTIMATE)
    return estimates::creators::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_INITIALISATION_PHASE || object_type == PARAM_INITIALISATION_PHASES)
    return initialisationphases::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_LENGTH_WEIGHT || object_type == PARAM_LENGTH_WEIGHTS)
    return lengthweights::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_LIKELIHOOD)
    return likelihoods::Factory::Create(sub_type);
  else if (object_type == PARAM_MINIMIZER)
    return minimisers::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_MCMC)
    return mcmcs::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_MODEL)
    return Model::Instance();
  else if (object_type == PARAM_OBSERVATION)
    return observations::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_PENALTY)
    return penalties::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_PROCESS || object_type == PARAM_PROCESSES)
    return processes::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_PROFILE)
    return profiles::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_PROJECT || object_type == PARAM_PROJECTS)
    return projects::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_AGEING || object_type == PARAM_MATURATION || object_type == PARAM_MORTALITY || object_type == PARAM_RECRUITMENT)
    return processes::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_STATE || object_type == PARAM_TAG || object_type == PARAM_TRANSITION) // @process specialisation
    return processes::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_REPORT)
    return reports::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_SELECTIVITY || object_type == PARAM_SELECTIVITIES)
    return selectivities::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_TIME_STEP || object_type == PARAM_TIME_STEPS)
    return timesteps::Factory::Create(model_, object_type, sub_type);
  else if (object_type == PARAM_TIME_VARYING)
    return timevarying::Factory::Create(model_, object_type, sub_type);

  return nullptr;
}

} /* namespace niwa */
