/**
 * @file Multiplicative.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_OBSERVATIONS_DATAWEIGHTS_CHILDREN_MULTIPLICATIVE_H_
#define SOURCE_OBSERVATIONS_DATAWEIGHTS_CHILDREN_MULTIPLICATIVE_H_

// headers
#include "Observations/DataWeights/DataWeight.h"

// namespaces
namespace niwa {
namespace observations {
namespace dataweights {

/**
 *
 */
class Multiplicative : public observations::DataWeight {
public:
  // methods
  Multiplicative() = default;
  virtual                     ~Multiplicative() = default;
  Double                      AdjustLikelihood(Double likelihood) override final;

  // accessors
  bool                        set_weight_value(Double new_value);
};

} /* namespace dataweights */
} /* namespace observations */
} /* namespace niwa */

#endif /* SOURCE_OBSERVATIONS_DATAWEIGHTS_CHILDREN_MULTIPLICATIVE_H_ */
