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
#include "Priors/Manager.h"
#include "Priors/Children/Beta.h"
#include "Priors/Children/LogNormal.h"
#include "Priors/Children/Normal.h"
#include "Priors/Children/NormalByStdev.h"
#include "Priors/Children/NormalLog.h"
#include "Priors/Children/Uniform.h"
#include "Priors/Children/UniformLog.h"

// Namespaces
namespace isam {
namespace priors {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
PriorPtr Factory::Create(const string& object_type, const string& sub_type) {
  PriorPtr result;

  if (object_type == PARAM_PRIOR) {
    if (sub_type == PARAM_BETA)
      result = PriorPtr(new Beta());
    else if (sub_type == PARAM_LOG_NORMAL)
      result = PriorPtr(new LogNormal());
    else if (sub_type == PARAM_NORMAL)
      result = PriorPtr(new Normal());
    else if (sub_type == PARAM_NORMAL_BY_STDEV)
      result = PriorPtr(new NormalByStdev());
    else if (sub_type == PARAM_NORMAL_LOG)
      result = PriorPtr(new NormalLog());
    else if (sub_type == PARAM_UNIFORM)
      result = PriorPtr(new Uniform());
    else if (sub_type == PARAM_UNIFORM_LOG)
      result = PriorPtr(new UniformLog());

    if (result)
      isam::priors::Manager::Instance().AddObject(result);
  }

  return result;
}

} /* namespace priors */
} /* namespace isam */
