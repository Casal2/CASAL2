/**
 * @file Base.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2/09/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include "Base.h"

#include "AgeingErrors/Manager.h"
#include "AgeLengths/Manager.h"
#include "AgeLengthKeys/Manager.h"
#include "Asserts/Manager.h"
#include "Catchabilities/Manager.h"
#include "Categories/Categories.h"
#include "DerivedQuantities/Manager.h"
#include "Estimates/Manager.h"
#include "InitialisationPhases/Manager.h"
#include "MCMCs/Manager.h"
#include "Minimisers/Manager.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Observations/Manager.h"
#include "Partition/Accessors/Age/Category.h"
#include "Partition/Partition.h"
#include "Penalties/Manager.h"
#include "Processes/Manager.h"
#include "Projects/Manager.h"
#include "Reports/Manager.h"
#include "Selectivities/Factory.h"
#include "Selectivities/Manager.h"
#include "LengthWeights/Manager.h"
#include "TimeSteps/Manager.h"
#include "Logging/Logging.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace testfixtures {

/**
 *
 */
Base::Base() { }

/**
 *
 */
Base::~Base() { }

/**
 *
 */
void Base::SetUp() { }

/**
 *
 */
void Base::TearDown() {
  Model::Instance(true);

  Categories::Instance()->RemoveAllObjects();
  Partition::Instance().Clear();

  ageingerrors::Manager::Instance().RemoveAllObjects();
  agelengths::Manager::Instance().RemoveAllObjects();
  agelengthkeys::Manager::Instance().RemoveAllObjects();
  asserts::Manager::Instance().RemoveAllObjects();
  catchabilities::Manager::Instance().RemoveAllObjects();
  derivedquantities::Manager::Instance().RemoveAllObjects();
  estimates::Manager::Instance().RemoveAllObjects();
  initialisationphases::Manager::Instance().RemoveAllObjects();
  minimisers::Manager::Instance().RemoveAllObjects();
  observations::Manager::Instance().RemoveAllObjects();
  penalties::Manager::Instance().RemoveAllObjects();
  processes::Manager::Instance().RemoveAllObjects();
  projects::Manager::Instance().RemoveAllObjects();
  reports::Manager::Instance().RemoveAllObjects();
  selectivities::Manager::Instance().RemoveAllObjects();
  lengthweights::Manager::Instance().RemoveAllObjects();
  timesteps::Manager::Instance().RemoveAllObjects();

  mcmcs::Manager::Instance().RemoveAllObjects();

  niwa::ObjectiveFunction::Instance().Clear();
}

} /* namespace testfixtures */
} /* namespace niwa */
#endif /* TESTMODE */
