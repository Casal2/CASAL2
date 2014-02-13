/**
 * @file Comparison.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 4/02/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef OBSERVATIONS_COMPARISON_H_
#define OBSERVATIONS_COMPARISON_H_

// headers
#include <string>

#include "Utilities/Types.h"

// namespaces
namespace isam {
namespace observations {

using std::string;
using utilities::Double;

struct Comparison {
  string    category_ = "";
  unsigned  age_ = 0;
  Double    expected_ = 0;
  Double    observed_ = 0;
  Double    error_value_ = 0;
  Double    process_error_ = 0;
  Double    delta_ = 0;
  Double    score_ = 0;
};

} /* namespace observations */
} /* namespace isam */
#endif /* OBSERVATIONS_COMPARISON_H_ */
