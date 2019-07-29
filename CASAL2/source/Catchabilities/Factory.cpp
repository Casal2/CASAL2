/**
 * @file Factory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 12/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "Model/Model.h"
#include "Model/Managers.h"
#include "Catchabilities/Manager.h"
#include "Catchabilities/Common/Free.h"
#include "Catchabilities/Common/Nuisance.h"

// namespaces
namespace niwa {
namespace catchabilities {

/**
 * Create a catchability
 *
 * @param object_type The type of object
 * @param sub_type The sub type (e.g beta)
 * @return ptr of an catchability
 */
Catchability* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  Catchability* result = nullptr;

  if (object_type == PARAM_CATCHABILITY) {
    if (sub_type == PARAM_FREE)
      result = new Free(model);
    else if (sub_type == PARAM_NUISANCE)
      result = new Nuisance(model);

    if (result)
      model->managers().catchability()->AddObject(result);
  }

  return result;
}

} /* catchabilities */
} /* niwa */
