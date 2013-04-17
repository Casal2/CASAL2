/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Factory.h"

#include "Minimisers/Children/DESolver.h"
#include "Minimisers/Children/GammaDiff.h"
#include "Minimisers/Manager.h"

// Namespaces
namespace isam {
namespace minimisers {

/**
 * Create the specified type of minimiser and add it to the manager
 *
 * @param block_type The @block type used in the configuration file
 * @param object_type The 'type' of minimiser we want to create
 * @return A shared_ptr<> to the minimiser we've created
 */
MinimiserPtr Factory::Create(const string& block_type, const string& object_type) {
  MinimiserPtr result;

  if (block_type == PARAM_MINIMIZER) {
    if (object_type == PARAM_DE_SOLVER) {
      result = MinimiserPtr(new DESolver());

    } else if (object_type == PARAM_GAMMADIFF) {
      result = MinimiserPtr(new GammaDiff());

    }
  }

  if (result)
    isam::minimisers::Manager::Instance().AddObject(result);

  return result;
}

} /* namespace minimisers */
} /* namespace isam */
