/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 3/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "Estimates/Transformations/Children/Log.h"
#include "Estimates/Transformations/Children/Inverse.h"
#include "Estimates/Transformations/Children/SquareRoot.h"
#include "Translations/Translations.h"

// namespaces
namespace niwa {
namespace estimates {
namespace transformations {

/**
 * Create the estimate transformation
 *
 * @param object_type The type of transformation to create
 * @return a shared_ptr to the estimate transformation
 */
Transformation* Factory::Create(const string& sub_type) {
  Transformation* result = nullptr;

  if (sub_type == PARAM_LOG)
    result = new Log();
  else if (sub_type == PARAM_INVERSE)
    result = new Inverse();
  else if (sub_type == PARAM_SQUARE_ROOT)
    result = new SquareRoot();

  return result;
}

} /* namespace transformations */
} /* namespace estimates */
} /* namespace niwa */
