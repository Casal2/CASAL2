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
#ifndef SOURCE_OBSERVATIONS_PROCESSERRORS_PROCESSERROR_H_
#define SOURCE_OBSERVATIONS_PROCESSERRORS_PROCESSERROR_H_

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
class ProcessError {
public:
  ProcessError() = default;
  virtual                     ~ProcessError() = default;
  virtual Double              CalculateScore(Double process_error, Double error_value, Double likelihood) = 0;
};

// typedef
typedef boost::shared_ptr<observations::ProcessError> ProcessErrorPtr;

} /* namespace observations */
} /* namespace niwa */

#endif /* SOURCE_OBSERVATIONS_PROCESSERRORS_PROCESSERROR_H_ */
