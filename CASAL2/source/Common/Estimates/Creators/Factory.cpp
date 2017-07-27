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

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"
#include "Common/Estimates/Manager.h"
#include "Common/Estimates/Creators/Children/Beta.h"
#include "Common/Estimates/Creators/Children/Lognormal.h"
#include "Common/Estimates/Creators/Children/Normal.h"
#include "Common/Estimates/Creators/Children/NormalByStdev.h"
#include "Common/Estimates/Creators/Children/NormalLog.h"
#include "Common/Estimates/Creators/Children/Uniform.h"
#include "Common/Estimates/Creators/Children/UniformLog.h"

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
Creator* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  Creator* object = nullptr;

  if (sub_type == PARAM_BETA)
    object = new Beta(model);
  else if (sub_type == PARAM_LOGNORMAL)
    object = new Lognormal(model);
  else if (sub_type == PARAM_NORMAL)
    object = new Normal(model);
  else if (sub_type == PARAM_NORMAL_BY_STDEV)
    object = new NormalByStdev(model);
  else if (sub_type == PARAM_NORMAL_LOG)
    object = new NormalLog(model);
  else if (sub_type == PARAM_UNIFORM)
    object = new Uniform(model);
  else if (sub_type == PARAM_UNIFORM_LOG)
    object = new UniformLog(model);

  if (object)
    model->managers().estimate()->AddCreator(object);

  return object;
}

} /* namespace creators */
} /* namespace estimates */
} /* namespace niwa */
