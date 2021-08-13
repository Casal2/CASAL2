/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Factory.h"

#include "../Minimisers/Manager.h"
#include "../Model/Managers.h"
#include "../Model/Model.h"

#ifdef USE_AUTODIFF
#ifdef USE_ADOLC
#include "../Minimisers/Common/ADOLC.h"
#elif defined(USE_BETADIFF)
#include "../Minimisers/Common/BetaDiff.h"
#endif
#endif

#include "../Minimisers/Common/Dummy/Dummy.h"

#ifndef USE_AUTODIFF
#include "../Minimisers/Common/DESolver.h"
#include "../Minimisers/Common/DeltaDiff.h"
#include "../Minimisers/Common/GammaDiff.h"
#endif

// Namespaces
namespace niwa {
namespace minimisers {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g., age_size, process)
 * @param sub_type The child type of the object to create (e.g., ageing, schnute)
 * @return shared_ptr to the object
 */
Minimiser* Factory::Create(shared_ptr<Model> model, const string& object_type, const string& sub_type) {
  Minimiser* result = nullptr;

  if (object_type == PARAM_MINIMIZER) {
#ifdef USE_AUTODIFF
#ifdef USE_BETADIFF
    if (sub_type == PARAM_BETADIFF) {
      result = new BetaDiff(model);
    }
#elif defined(USE_ADOLC)
    if (sub_type == PARAM_ADOLC) {
      result = new ADOLC(model);
    }
#endif
    else if (sub_type == PARAM_DE_SOLVER || sub_type == PARAM_GAMMADIFF || sub_type == PARAM_NUMERICAL_DIFFERENCES)
      result = new Dummy(model);
#endif

#ifndef USE_AUTODIFF
    if (sub_type == PARAM_DELTADIFF)
      result = new DeltaDiff(model);
    else if (sub_type == PARAM_DE_SOLVER)
      result = new DESolver(model);
    else if (sub_type == PARAM_GAMMADIFF || sub_type == PARAM_NUMERICAL_DIFFERENCES)
      result = new GammaDiff(model);
    else if (sub_type == PARAM_BETADIFF || sub_type == PARAM_ADOLC)
      result = new Dummy(model);
#endif

    if (result)
      model->managers()->minimiser()->AddObject(result);
  }

  return result;
}

} /* namespace minimisers */
} /* namespace niwa */
