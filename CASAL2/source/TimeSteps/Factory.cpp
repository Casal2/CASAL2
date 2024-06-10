/**
 * @file Factory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 9/10/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// Headers
#include "Factory.h"

#include "../Model/Managers.h"
#include "../Model/Model.h"
#include "../TimeSteps/Manager.h"

// Namespaces
namespace niwa {
namespace timesteps {

/**
 * Create the instance of the object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g., age_size, process)
 * @param sub_type The child type of the object to create (e.g., ageing, schnute)
 * @return pointer to the object
 */
TimeStep* Factory::Create(shared_ptr<Model> model, const string& object_type, const string& sub_type) {
  LOG_TRACE();
  TimeStep* object = nullptr;
  if (object_type == PARAM_TIME_STEP || object_type == PARAM_TIME_STEPS) {
    object = new TimeStep(model);

    if (object) {
      auto time_step_manager = model->managers()->time_step();
      if (!time_step_manager)
        LOG_CODE_ERROR() << "!time_step_manager";
      time_step_manager->AddObject(object);
    }
  }

  return object;
}

} /* namespace timesteps */
} /* namespace niwa */
