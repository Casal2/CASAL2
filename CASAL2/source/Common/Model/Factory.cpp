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

#include "Common/BaseClasses/Object.h"
#include "Common/AdditionalPriors/Factory.h"
#include "Age/AgeingErrors/Factory.h"
#include "Age/AgeLengths/Factory.h"
#include "Common/Asserts/Factory.h"
#include "Common/Catchabilities/Factory.h"
#include "Common/Categories/Categories.h"
#include "Common/DerivedQuantities/Factory.h"
#include "Common/Estimates/Creators/Factory.h"
#include "Common/EstimateTransformations/Factory.h"
#include "Common/InitialisationPhases/Factory.h"
#include "Common/Likelihoods/Factory.h"
#include "Common/MCMCs/Factory.h"
#include "Common/Minimisers/Factory.h"
#include "Common/Model/Model.h"
#include "Common/Observations/Factory.h"
#include "Common/Penalties/Factory.h"
#include "Common/Processes/Factory.h"
#include "Common/Profiles/Factory.h"
#include "Common/Projects/Factory.h"
#include "Common/Reports/Factory.h"
#include "Age/Selectivities/Factory.h"
#include "Length/LengthWeights/Factory.h"
#include "Common/TimeSteps/Factory.h"
#include "Common/TimeVarying/Factory.h"
#include "Common/Utilities/To.h"

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
base::Object* Factory::CreateObject(const string& object_type, const string& sub_type) {
  string lwr_object_type = utilities::ToLowercase(object_type);
  string lwr_sub_type    = utilities::ToLowercase(sub_type);

  if (lwr_object_type == PARAM_ADDITIONAL_PRIOR)
    return additionalpriors::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_AGEING_ERROR || lwr_object_type == PARAM_AGEING_ERRORS)
    return ageingerrors::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_AGE_LENGTH || lwr_object_type == PARAM_AGE_LENGTHS)
    return agelengths::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_ASSERT)
    return asserts::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_CATCHABILITY)
    return catchabilities::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_CATEGORIES)
    return model_->categories();
  else if (lwr_object_type == PARAM_DERIVED_QUANTITY || lwr_object_type == PARAM_DERIVED_QUANTITIES)
    return derivedquantities::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_ESTIMATE)
    return estimates::creators::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_ESTIMATE_TRANSFORMATION)
    return estimatetransformations::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_INITIALISATION_PHASE || lwr_object_type == PARAM_INITIALISATION_PHASES)
    return initialisationphases::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_LENGTH_WEIGHT || lwr_object_type == PARAM_LENGTH_WEIGHTS)
    return lengthweights::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_LIKELIHOOD)
    return likelihoods::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_MINIMIZER)
    return minimisers::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_MCMC)
    return mcmcs::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_MODEL)
    return model_;
  else if (lwr_object_type == PARAM_OBSERVATION)
    return observations::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_PENALTY)
    return penalties::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_PROCESS || lwr_object_type == PARAM_PROCESSES)
    return processes::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_PROFILE)
    return profiles::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_PROJECT || lwr_object_type == PARAM_PROJECTS)
    return projects::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_AGEING || lwr_object_type == PARAM_MATURATION || lwr_object_type == PARAM_MORTALITY || lwr_object_type == PARAM_RECRUITMENT)
    return processes::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_STATE || lwr_object_type == PARAM_TAG || lwr_object_type == PARAM_TRANSITION) // @process specialisation
    return processes::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_REPORT)
    return reports::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_SELECTIVITY || lwr_object_type == PARAM_SELECTIVITIES)
    return selectivities::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_TIME_STEP || lwr_object_type == PARAM_TIME_STEPS)
    return timesteps::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_TIME_VARYING)
    return timevarying::Factory::Create(model_, lwr_object_type, lwr_sub_type);

  return nullptr;
}

} /* namespace niwa */
