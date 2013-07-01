/**
 * @file EmptyModel.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 9/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include "EmptyModel.h"

#include "Catchabilities/Manager.h"
#include "Categories/Categories.h"
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
#include "TimeSteps/Manager.h"
#include "Utilities/Logging/Logging.h"
#include "Utilities/To.h"

namespace isam {
namespace testfixtures {

EmptyModel::EmptyModel() {
}

EmptyModel::~EmptyModel() {
}

void EmptyModel::SetUp() {
  Model::Instance()->set_run_mode(RunMode::kTesting);
}

void EmptyModel::TearDown() {
  /**
   * Clean our Model
   */
  Model::Instance(true);

  Categories::Instance()->RemoveAllObjects();
  Partition::Instance().Clear();

  catchabilities::Manager::Instance().RemoveAllObjects();
  estimates::Manager::Instance().RemoveAllObjects();
  initialisationphases::Manager::Instance().RemoveAllObjects();
  minimisers::Manager::Instance().RemoveAllObjects();
  observations::Manager::Instance().RemoveAllObjects();
  penalties::Manager::Instance().RemoveAllObjects();
  priors::Manager::Instance().RemoveAllObjects();
  processes::Manager::Instance().RemoveAllObjects();
  reports::Manager::Instance().RemoveAllObjects();
  selectivities::Manager::Instance().RemoveAllObjects();
  timesteps::Manager::Instance().RemoveAllObjects();
}

} /* namespace testfixtures */
} /* namespace isam */
#endif
