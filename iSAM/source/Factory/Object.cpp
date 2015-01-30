/**
 * @file Object.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 7/10/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Object.h"

#include "AdditionalPriors/Factory.h"
#include "AgeSizes/Factory.h"
#include "Asserts/Factory.h"
#include "Catchabilities/Factory.h"
#include "Categories/Categories.h"
#include "DerivedQuantities/Factory.h"
#include "Estimates/Creators/Factory.h"
#include "InitialisationPhases/Factory.h"
#include "Likelihoods/Factory.h"
#include "MCMC/MCMC.h"
#include "Minimisers/Factory.h"
#include "Model/Model.h"
#include "Observations/Factory.h"
#include "Penalties/Factory.h"
#include "Processes/Factory.h"
#include "Profiles/Factory.h"
#include "Projects/Factory.h"
#include "Reports/Factory.h"
#include "Selectivities/Factory.h"
#include "SizeWeights/Factory.h"
#include "TimeSteps/Factory.h"
#include "TimeVarying/Factory.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace factory {

using base::ObjectPtr;

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
base::ObjectPtr Object::Create(string& object_type, string& sub_type) {
  ObjectPtr result;

  object_type = utilities::ToLowercase(object_type);
  sub_type    = utilities::ToLowercase(sub_type);

  if (object_type == PARAM_ADDITIONAL_PRIOR)
    result = additionalpriors::Factory::Create(object_type, sub_type);
  else if (object_type == PARAM_AGE_SIZE || object_type == PARAM_AGE_SIZES)
    result = agesizes::Factory::Create(object_type, sub_type);
  else if (object_type == PARAM_ASSERT)
    result = asserts::Factory::Create(object_type, sub_type);
  else if (object_type == PARAM_CATCHABILITY)
    result = catchabilities::Factory::Create(object_type, sub_type);
  else if (object_type == PARAM_CATEGORIES)
    result = Categories::Instance();
  else if (object_type == PARAM_DERIVED_QUANTITY || object_type == PARAM_DERIVED_QUANTITIES)
    result = derivedquantities::Factory::Create(object_type, sub_type);
  else if (object_type == PARAM_ESTIMATE)
    result = estimates::creators::Factory::Create(object_type, sub_type);
  else if (object_type == PARAM_INITIALISATION_PHASE || object_type == PARAM_INITIALISATION_PHASES)
    result = initialisationphases::Factory::Create(object_type, sub_type);
  else if (object_type == PARAM_LIKELIHOOD)
    result = likelihoods::Factory::Create(sub_type);
  else if (object_type == PARAM_MINIMIZER)
    result = minimisers::Factory::Create(object_type, sub_type);
  else if (object_type == PARAM_MCMC)
    result = MCMC::Instance();
  else if (object_type == PARAM_MODEL)
    result = Model::Instance();
  else if (object_type == PARAM_OBSERVATION)
    result = observations::Factory::Create(object_type, sub_type);
  else if (object_type == PARAM_PENALTY)
    result = penalties::Factory::Create(object_type, sub_type);
  else if (object_type == PARAM_PROCESS || object_type == PARAM_PROCESSES)
    result = processes::Factory::Create(object_type, sub_type);
  else if (object_type == PARAM_PROFILE)
    result = profiles::Factory::Create();
  else if (object_type == PARAM_PROJECT || object_type == PARAM_PROJECTS)
    result = projects::Factory::Create(object_type, sub_type);
  else if (object_type == PARAM_AGEING || object_type == PARAM_MATURATION || object_type == PARAM_MORTALITY || object_type == PARAM_RECRUITMENT)
    result = processes::Factory::Create(object_type, sub_type);
  else if (object_type == PARAM_TRANSITION) // @process specialisation
    result = processes::Factory::Create(object_type, sub_type);
  else if (object_type == PARAM_REPORT)
    result = reports::Factory::Create(object_type, sub_type);
  else if (object_type == PARAM_SELECTIVITY || object_type == PARAM_SELECTIVITIES)
    result = selectivities::Factory::Create(object_type, sub_type);
  else if (object_type == PARAM_SIZE_WEIGHT || object_type == PARAM_SIZE_WEIGHTS)
    result = sizeweights::Factory::Create(object_type, sub_type);
  else if (object_type == PARAM_TIME_STEP || object_type == PARAM_TIME_STEPS)
    result = timesteps::Factory::Create();
  else if (object_type == PARAM_TIME_VARYING)
    result = timevarying::Factory::Create(object_type, sub_type);

  return result;
}

} /* namespace factory */
} /* namespace niwa */
