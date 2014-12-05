/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 22/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 */

// headers
#include "Factory.h"

#include "Estimates/Manager.h"
#include "Estimates/Creators/Children/Beta.h"
#include "Estimates/Creators/Children/Lognormal.h"
#include "Estimates/Creators/Children/Normal.h"
#include "Estimates/Creators/Children/NormalByStdev.h"
#include "Estimates/Creators/Children/NormalLog.h"
#include "Estimates/Creators/Children/Uniform.h"
#include "Estimates/Creators/Children/UniformLog.h"

// namespaces
namespace niwa {
namespace estimates {
namespace creators {

/**
 * Create an instance of a Creator object defined by the object and sub type
 *
 * @param object_type The major object type (@estimate)
 * @param sub_type The sub type of the object (e.g beta)
 */
CreatorPtr Factory::Create(const string& object_type, const string& sub_type) {
  CreatorPtr object;

  if (sub_type == PARAM_BETA)
    object = CreatorPtr(new Beta());
  else if (sub_type == PARAM_LOGNORMAL)
    object = CreatorPtr(new Lognormal());
  else if (sub_type == PARAM_NORMAL)
    object = CreatorPtr(new Normal());
  else if (sub_type == PARAM_NORMAL_BY_STDEV)
    object = CreatorPtr(new NormalByStdev());
  else if (sub_type == PARAM_NORMAL_LOG)
    object = CreatorPtr(new NormalLog());
  else if (sub_type == PARAM_UNIFORM)
    object = CreatorPtr(new Uniform());
  else if (sub_type == PARAM_UNIFORM_LOG)
    object = CreatorPtr(new UniformLog());

  if (object)
    estimates::Manager::Instance().AddCreator(object);

  return object;
}

} /* namespace creators */
} /* namespace estimates */
} /* namespace niwa */
