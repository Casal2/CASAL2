/**
 * @file Factory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Dec 7, 2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "../Model/Managers.h"
#include "../Model/Model.h"
#include "../EstimateTransformations/Manager.h"
#include "../EstimateTransformations/Common/AverageDifference.h"
#include "../EstimateTransformations/Common/Orthogonal.h"
#include "../EstimateTransformations/Common/Inverse.h"
#include "../EstimateTransformations/Common/Log.h"
#include "../EstimateTransformations/Common/LogSum.h"
#include "../EstimateTransformations/Common/SquareRoot.h"
#include "../EstimateTransformations/Common/SumToOne.h"
#include "../EstimateTransformations/Common/Simplex.h"
// namespaces
namespace niwa {
namespace estimatetransformations {

/**
 * Create the instance of the object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g., age_size, process)
 * @param sub_type The child type of the object to create (e.g., ageing, schnute)
 * @return shared_ptr to the object
 */
EstimateTransformation* Factory::Create(shared_ptr<Model> model, const string& object_type, const string& sub_type) {
  EstimateTransformation* result = nullptr;

  if (object_type == PARAM_ESTIMATE_TRANSFORMATION) {
    if (sub_type == PARAM_INVERSE)
      result = new Inverse(model);
    else if (sub_type == PARAM_LOG)
      result = new Log(model);
    else if (sub_type == PARAM_LOG_SUM)
      result = new LogSum(model);
    else if (sub_type == PARAM_ORTHOGONAL)
      result = new Orthogonal(model);
    else if (sub_type == PARAM_SQUARE_ROOT)
      result = new SquareRoot(model);
    else if (sub_type == PARAM_SIMPLEX)
      result = new Simplex(model);
    else if (sub_type == PARAM_AVERAGE_DIFFERENCE)
      result = new AverageDifference(model);
    else if (sub_type == PARAM_SUM_TO_ONE)
      result = new SumToOne(model);

    if (result)
      model->managers()->estimate_transformation()->AddObject(result);
  }

  return result;
}

} /* namespace estimatetransformations */
} /* namespace niwa */
