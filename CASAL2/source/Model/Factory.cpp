/**
 * @file Factory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 8/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

// Common Factories
#include "../AdditionalPriors/Factory.h"
#include "../Asserts/Factory.h"
#include "../BaseClasses/Object.h"
#include "../Catchabilities/Factory.h"
#include "../Categories/Categories.h"
#include "../DerivedQuantities/Factory.h"
#include "../EstimateTransformations/Factory.h"
#include "../Estimates/Creators/Factory.h"
#include "../InitialisationPhases/Factory.h"
#include "../LengthWeights/Factory.h"
#include "../Likelihoods/Factory.h"
#include "../MCMCs/Factory.h"
#include "../Minimisers/Factory.h"
#include "../Model/Model.h"
#include "../Model/Models/Age.h"
#include "../Model/Models/Length.h"
#include "../Model/Models/Multivariate.h"
#include "../Model/Models/PiApproximation.h"
#include "../Observations/Factory.h"
#include "../Penalties/Factory.h"
#include "../Processes/Factory.h"
#include "../Profiles/Factory.h"
#include "../Projects/Factory.h"
#include "../Reports/Factory.h"
#include "../Selectivities/Factory.h"
#include "../TimeSteps/Factory.h"
#include "../TimeVarying/Factory.h"
#include "../Utilities/To.h"

// Age Factories
#include "../AgeLengths/Factory.h"
#include "../AgeWeights/Factory.h"
#include "../AgeingErrors/Factory.h"

// Length Factories
// namespaces
namespace niwa {

/**
 * Default constructor
 */
Factory::Factory(shared_ptr<Model> model) : model_(model) {}

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
 * @param partition_type The specifically defined partition type for this object
 * @return A shared_ptr to the object we've created
 */
base::Object* Factory::CreateObject(const string& object_type, const string& sub_type, PartitionType partition_type) {
  string lwr_object_type = utilities::ToLowercase(object_type);
  string lwr_sub_type    = utilities::ToLowercase(sub_type);

  if (lwr_object_type == PARAM_ADDITIONAL_PRIOR)
    return additionalpriors::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_AGEING_ERROR || lwr_object_type == PARAM_AGEING_ERRORS)
    return ageingerrors::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_AGE_LENGTH || lwr_object_type == PARAM_AGE_LENGTHS)
    return agelengths::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_AGE_WEIGHT)
    return ageweights::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_ASSERT)
    return asserts::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_CATCHABILITY)
    return catchabilities::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_CATEGORIES)
    return model_->categories();
  else if (lwr_object_type == PARAM_DERIVED_QUANTITY || lwr_object_type == PARAM_DERIVED_QUANTITIES)
    return derivedquantities::Factory::Create(model_, lwr_object_type, lwr_sub_type, partition_type);
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
    return model_.get();
  else if (lwr_object_type == PARAM_OBSERVATION)
    return observations::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_PENALTY)
    return penalties::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_PROCESS || lwr_object_type == PARAM_PROCESSES)
    return processes::Factory::Create(model_, lwr_object_type, lwr_sub_type, partition_type);
  else if (lwr_object_type == PARAM_PROFILE)
    return profiles::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_PROJECT || lwr_object_type == PARAM_PROJECTS)
    return projects::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_AGEING || lwr_object_type == PARAM_MATURATION || lwr_object_type == PARAM_MORTALITY || lwr_object_type == PARAM_RECRUITMENT)
    return processes::Factory::Create(model_, lwr_object_type, lwr_sub_type, partition_type);
  else if (lwr_object_type == PARAM_STATE || lwr_object_type == PARAM_TAG || lwr_object_type == PARAM_TRANSITION)  // @process specialisation
    return processes::Factory::Create(model_, lwr_object_type, lwr_sub_type, partition_type);
  else if (lwr_object_type == PARAM_REPORT)
    return reports::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_SELECTIVITY || lwr_object_type == PARAM_SELECTIVITIES)
    return selectivities::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_LOSS_RATE_SELECTIVITIES || lwr_object_type == PARAM_RELATIVE_M_BY_AGE)
    return selectivities::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_TIME_STEP || lwr_object_type == PARAM_TIME_STEPS)
    return timesteps::Factory::Create(model_, lwr_object_type, lwr_sub_type);
  else if (lwr_object_type == PARAM_TIME_VARYING)
    return timevarying::Factory::Create(model_, lwr_object_type, lwr_sub_type);

  LOG_FINE() << "Couldn't find appropriate factory";
  return nullptr;
}

/**
 * Factory class to create different model types
 */
shared_ptr<Model> Factory::Create(const string& object_type, const string& sub_type) {
  shared_ptr<Model> result;

  if (object_type == PARAM_MODEL) {
    if (sub_type == PARAM_AGE || sub_type == "") {
      result.reset(new model::Age());
    } else if (sub_type == PARAM_LENGTH) {
      result.reset(new model::Length());
#ifndef USE_AUTODIFF
    } else if (sub_type == PARAM_MULTIVARIATE) {
      result.reset(new model::Multivariate());
    } else if (sub_type == PARAM_PI_APPROX) {
      result.reset(new model::PiApproximation());
#endif
    }
  }

  if (result)
    return result;

  LOG_CODE_ERROR() << object_type << "." << sub_type << " is not valid";
  return nullptr;
}

} /* namespace niwa */
