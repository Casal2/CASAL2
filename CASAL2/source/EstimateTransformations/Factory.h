/**
 * @file Factory.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Dec 7, 2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_FACTORY_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_FACTORY_H_

// headers
#include <string>

#include "EstimateTransformations/EstimateTransformation.h"

// namespaces
namespace niwa {
namespace estimatetransformations {

// classes
class Factory {
public:
  static EstimateTransformation*  Create(Model* model, const string& object_type, const string& sub_type);
private:
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace estimatetransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_FACTORY_H_ */
