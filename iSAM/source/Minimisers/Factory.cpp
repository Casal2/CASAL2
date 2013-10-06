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

#include "Minimisers/Manager.h"

#ifdef USE_AUTODIFF
#ifdef USE_BETADIFF
#include "Minimisers/Children/BetaDiff.h"
#endif

#else
#include "Minimisers/Children/DESolver.h"
#include "Minimisers/Children/GammaDiff.h"
#endif

// Namespaces
namespace isam {
namespace minimisers {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
MinimiserPtr Factory::Create(const string& object_type, const string& sub_type) {
  MinimiserPtr result;

  if (object_type == PARAM_MINIMIZER) {
#ifdef USE_BETADIFF
    if (sub_type == PARAM_BETADIFF) {
      result = MinimiserPtr(new BetaDiff());
    }
#else
    if (sub_type == PARAM_DE_SOLVER)
      result = MinimiserPtr(new DESolver());
    else if (sub_type == PARAM_GAMMADIFF)
      result = MinimiserPtr(new GammaDiff());
#endif

    if (!result)
      LOG_ERROR("The minimiser " << object_type << "." << sub_type << " is not supported in the current configuration");
    if (result)
      minimisers::Manager::Instance().AddObject(result);
  }

  return result;
}

} /* namespace minimisers */
} /* namespace isam */
