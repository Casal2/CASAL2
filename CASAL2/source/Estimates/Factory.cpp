/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Factory.h"
#include "Estimates/Manager.h"
#include "Estimates/Children/Beta.h"
#include "Estimates/Children/Lognormal.h"
#include "Estimates/Children/Normal.h"
#include "Estimates/Children/NormalByStdev.h"
#include "Estimates/Children/NormalLog.h"
#include "Estimates/Children/Uniform.h"
#include "Estimates/Children/UniformLog.h"
#include "Model/Managers.h"
#include "Model/Model.h"

// Namespaces
namespace niwa {
namespace estimates {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
Estimate* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  Estimate* result = nullptr;

  if (object_type == PARAM_ESTIMATE) {
    if (sub_type == PARAM_BETA)
      result = new Beta();
    else if (sub_type == PARAM_LOGNORMAL)
      result = new Lognormal();
    else if (sub_type == PARAM_NORMAL)
      result = new Normal();
    else if (sub_type == PARAM_NORMAL_BY_STDEV)
      result = new NormalByStdev();
    else if (sub_type == PARAM_NORMAL_LOG)
      result = new NormalLog();
    else if (sub_type == PARAM_UNIFORM)
      result = new Uniform();
    else if (sub_type == PARAM_UNIFORM_LOG)
      result = new UniformLog();

    if (result)
      model->managers().estimate()->AddObject(result);
  }

  return result;
}

} /* namespace estimates */
} /* namespace niwa */
