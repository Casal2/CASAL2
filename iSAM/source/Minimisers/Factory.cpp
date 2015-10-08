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

#include "Model/Model.h"
#include "Model/Managers.h"
#include "Minimisers/Manager.h"

#ifdef USE_AUTODIFF
#ifdef USE_ADOLC
#include "Minimisers/Children/ADOLC.h"
#elif defined(USE_BETADIFF)
#include "Minimisers/Children/BetaDiff.h"
#elif defined(USE_CPPAD)
#include "Minimisers/Children/CPPAD.h"
#endif
#endif

#ifndef USE_AUTODIFF
#include "Minimisers/Children/DESolver.h"
#include "Minimisers/Children/DLib.h"
#include "Minimisers/Children/GammaDiff.h"
#endif

// Namespaces
namespace niwa {
namespace minimisers {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
Minimiser* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  Minimiser* result = nullptr;

  if (object_type == PARAM_MINIMIZER) {
#ifdef USE_BETADIFF
    if (sub_type == PARAM_BETADIFF) {
      result = new BetaDiff();
    }
#elif defined(USE_ADOLC)
    if (sub_type == PARAM_ADOLC) {
      result = new ADOLC();
    }
#elif defined(USE_CPPAD)
    if (sub_type == PARAM_CPPAD) {
      result = new CPPAD();
    }
#endif

#ifndef USE_AUTODIFF
    if (sub_type == PARAM_DE_SOLVER)
      result = new DESolver();
    else if (sub_type == PARAM_DLIB)
      result = new DLib();
    else if (sub_type == PARAM_GAMMADIFF)
      result = new GammaDiff();
#endif

    if (!result)
      LOG_ERROR() << "The minimiser " << object_type << "." << sub_type << " is not supported in the current configuration";
    if (result)
      model->managers().minimiser()->AddObject(result);
  }

  return result;
}

} /* namespace minimisers */
} /* namespace niwa */
