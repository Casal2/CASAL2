/**
 * @file Factory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "Model/Model.h"
#include "Model/Managers.h"
#include "TimeVarying/Common/AnnualShift.h"
#include "TimeVarying/Common/Constant.h"
#include "TimeVarying/Common/Exogenous.h"
#include "TimeVarying/Common/Linear.h"
#include "TimeVarying/Common/RandomDraw.h"
#include "TimeVarying/Common/RandomWalk.h"
#include "TimeVarying/Manager.h"

// namespaces
namespace niwa {
namespace timevarying {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
TimeVarying* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  TimeVarying* result = nullptr;

  if (object_type == PARAM_TIME_VARYING) {
    if (sub_type == PARAM_ANNUAL_SHIFT)
      result = new AnnualShift(model);
    else if (sub_type == PARAM_CONSTANT)
      result = new Constant(model);
    else if (sub_type == PARAM_EXOGENOUS)
      result = new Exogenous(model);
    else if (sub_type == PARAM_LINEAR)
      result = new Linear(model);
    else if (sub_type == PARAM_RANDOMWALK)
      result = new RandomWalk(model);
    else if (sub_type == PARAM_RANDOMDRAW)
      result = new RandomDraw(model);

    if (result)
      model->managers().time_varying()->AddObject(result);
  }

  return result;
}

} /* namespace projects */
} /* namespace niwa */
