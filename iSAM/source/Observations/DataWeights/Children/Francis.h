/**
 * @file Francis.h
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
#ifndef SOURCE_OBSERVATIONS_DATAWEIGHTS_CHILDREN_FRANCIS_H_
#define SOURCE_OBSERVATIONS_DATAWEIGHTS_CHILDREN_FRANCIS_H_

// headers
#include "Observations/DataWeights/DataWeight.h"

// namespaces
namespace niwa {
namespace observations {
namespace dataweights {

/**
 *
 */
class Francis : public observations::DataWeight {
public:
  // methods
  Francis() = default;
  virtual                     ~Francis() = default;
  Double                      AdjustErrorValue(Double error_value, Double process_error) override final;
  Double                      AdjustLikelihood(Double likelihood) override final;

  // accessors
  bool                set_weight_value(Double new_value) override final;
};

} /* namespace dataweights */
} /* namespace observations */
} /* namespace niwa */

#endif /* SOURCE_OBSERVATIONS_DATAWEIGHTS_CHILDREN_FRANCIS_H_ */
