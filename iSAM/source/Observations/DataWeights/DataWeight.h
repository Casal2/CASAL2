/**
 * @file ProcessError.h
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
#ifndef SOURCE_OBSERVATIONS_DATAWEIGHTS_DATAWEIGHT_H_
#define SOURCE_OBSERVATIONS_DATAWEIGHTS_DATAWEIGHT_H_

// headers
#include "BaseClasses/Object.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa {
namespace observations {

using utilities::Double;

/**
 * Class definition
 */
class DataWeight {
public:
  DataWeight() = default;
  virtual                     ~DataWeight() = default;
  virtual Double              AdjustErrorValue(Double error_value, Double process_error);
  virtual Double              AdjustLikelihood(Double likelihood) = 0;

  // accessors
  virtual bool                set_weight_value(Double new_value) { weight_value_ = new_value; return true; }
  Double                      weight_value() const { return weight_value_; }

protected:
  // members
  Double                      weight_value_ = 1.0;
};

// typedef
typedef boost::shared_ptr<observations::DataWeight> DataWeightPtr;

} /* namespace observations */
} /* namespace niwa */

#endif /* SOURCE_OBSERVATIONS_DATAWEIGHTS_DATAWEIGHT_H_ */
