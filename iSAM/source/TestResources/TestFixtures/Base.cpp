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
#include "Partition/Accessors/Category.h"
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

  Categories::Instance()->Clear();
  Partition::Instance().Clear();

  ageingerrors::Manager::Instance().Clear();
  agelengths::Manager::Instance().Clear();
  agelengthkeys::Manager::Instance().Clear();
  asserts::Manager::Instance().Clear();
  catchabilities::Manager::Instance().Clear();
  derivedquantities::Manager::Instance().Clear();
  estimates::Manager::Instance().Clear();
  initialisationphases::Manager::Instance().Clear();
  minimisers::Manager::Instance().Clear();
  observations::Manager::Instance().Clear();
  penalties::Manager::Instance().Clear();
  processes::Manager::Instance().Clear();
  projects::Manager::Instance().Clear();
  reports::Manager::Instance().Clear();
  selectivities::Manager::Instance().Clear();
  lengthweights::Manager::Instance().Clear();
  timesteps::Manager::Instance().Clear();

  mcmcs::Manager::Instance().Clear();

  niwa::ObjectiveFunction::Instance().Clear();
}

} /* namespace testfixtures */
} /* namespace niwa */
#endif /* TESTMODE */
