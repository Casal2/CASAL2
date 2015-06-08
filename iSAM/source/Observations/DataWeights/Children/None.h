/**
 * @file None.h
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
#ifndef SOURCE_OBSERVATIONS_DATAWEIGHTS_CHILDREN_NONE_H_
#define SOURCE_OBSERVATIONS_DATAWEIGHTS_CHILDREN_NONE_H_

// headers
#include "Observations/DataWeights/DataWeight.h"

// namespaces
namespace niwa {
namespace observations {
namespace dataweights {

/**
 * Class definition
 */
class None : public observations::DataWeight {
public:
  // methods
  None() = default;
  virtual                     ~None() = default;
  Double                      AdjustLikelihood(Double likelihood) override final { return likelihood; }
};

} /* namespace dataweights */
} /* namespace observations */
} /* namespace niwa */

#endif /* SOURCE_OBSERVATIONS_DATAWEIGHTS_CHILDREN_NONE_H_ */
