/**
 * @file AdditiveNormal.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 22/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_OBSERVATIONS_PROCESSERRORS_CHILDREN_ADDITIVENORMAL_H_
#define SOURCE_OBSERVATIONS_PROCESSERRORS_CHILDREN_ADDITIVENORMAL_H_

// headers
#include "Observations/ProcessErrors/ProcessError.h"

// namespaces
namespace niwa {
namespace observations {
namespace processerrors {

/**
 * Class definition
 */
class AdditiveNormal : public observations::ProcessError {
public:
  AdditiveNormal() = default;
  virtual                     ~AdditiveNormal() = default;
  Double                      CalculateScore(Double process_error, Double error_value, Double likelihood) override final;
};

} /* namespace processerrors */
} /* namespace observations */
} /* namespace niwa */

#endif /* SOURCE_OBSERVATIONS_PROCESSERRORS_CHILDREN_ADDITIVENORMAL_H_ */
