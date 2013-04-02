/**
 * @file BasicModel.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 2/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include "BasicModel.h"

// Namespaces
namespace isam {
namespace testfixtures {

/**
 * Constructor
 */
BasicModel::BasicModel() {

}

/**
 * Destructor
 */
BasicModel::~BasicModel() {
  /**
   * Clean our Model
   */
//  Categories::Instance()->();
  Partition::Instance().

//  catchabilities::Manager::Instance().RemoveAllObjects();
//  estimates::Manager::Instance().RemoveAllObjects();
//  initialisationphases::Manager::Instance().RemoveAllObjects();
//  minimisers::Manager::Instance().RemoveAllObjects();
//  observations::Manager::Instance().RemoveAllObjects();
//  penalties::Manager::Instance().RemoveAllObjects();
//  priors::Manager::Instance().RemoveAllObjects();
//  processes::Manager::Instance().RemoveAllObjects();
//  reports::Manager::Instance().RemoveAllObjects();
//  selectivities::Manager::Instance().RemoveAllObjects();
//  timesteps::Manager::Instance().RemoveAllObjects();
}

} /* namespace testfixtures */
} /* namespace isam */

#endif /* TESTMODE */
