/**
 * @file Base.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2/09/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Base.h"

#include "AgeSizes/Manager.h"
#include "Catchabilities/Manager.h"
#include "Categories/Categories.h"
#include "DerivedQuantities/Manager.h"
#include "Estimates/Manager.h"
#include "InitialisationPhases/Manager.h"
#include "Minimisers/Manager.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Observations/Manager.h"
#include "Partition/Accessors/Category.h"
#include "Partition/Partition.h"
#include "Penalties/Manager.h"
#include "Priors/Manager.h"
#include "Processes/Manager.h"
#include "Reports/Manager.h"
#include "Selectivities/Factory.h"
#include "Selectivities/Manager.h"
#include "SizeWeights/Manager.h"
#include "TimeSteps/Manager.h"
#include "Utilities/Logging/Logging.h"
#include "Utilities/To.h"

// namespaces
namespace isam {
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

  agesizes::Manager::Instance().RemoveAllObjects();
  catchabilities::Manager::Instance().RemoveAllObjects();
  derivedquantities::Manager::Instance().RemoveAllObjects();
  estimates::Manager::Instance().RemoveAllObjects();
  initialisationphases::Manager::Instance().RemoveAllObjects();
  minimisers::Manager::Instance().RemoveAllObjects();
  observations::Manager::Instance().RemoveAllObjects();
  penalties::Manager::Instance().RemoveAllObjects();
  priors::Manager::Instance().RemoveAllObjects();
  processes::Manager::Instance().RemoveAllObjects();
  reports::Manager::Instance().RemoveAllObjects();
  selectivities::Manager::Instance().RemoveAllObjects();
  sizeweights::Manager::Instance().RemoveAllObjects();
  timesteps::Manager::Instance().RemoveAllObjects();

}

} /* namespace testfixtures */
} /* namespace isam */
