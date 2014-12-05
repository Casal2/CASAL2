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
EstimatePtr Factory::Create(const string& object_type, const string& sub_type) {
  EstimatePtr result;

  if (object_type == PARAM_ESTIMATE) {
    if (sub_type == PARAM_BETA)
      result = EstimatePtr(new Beta());
    else if (sub_type == PARAM_LOGNORMAL)
      result = EstimatePtr(new Lognormal());
    else if (sub_type == PARAM_NORMAL)
      result = EstimatePtr(new Normal());
    else if (sub_type == PARAM_NORMAL_BY_STDEV)
      result = EstimatePtr(new NormalByStdev());
    else if (sub_type == PARAM_NORMAL_LOG)
      result = EstimatePtr(new NormalLog());
    else if (sub_type == PARAM_UNIFORM)
      result = EstimatePtr(new Uniform());
    else if (sub_type == PARAM_UNIFORM_LOG)
      result = EstimatePtr(new UniformLog());

    if (result)
      niwa::estimates::Manager::Instance().AddObject(result);
  }

  return result;
}

} /* namespace estimates */
} /* namespace niwa */
