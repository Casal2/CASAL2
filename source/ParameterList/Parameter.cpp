/**
 * @file Parameter.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Parameter.h"

#include "Utilities/Logging/Logging.h"
#include "Utilities/To.h"

// Namespaces
namespace isam {
namespace parameterlist {

string Parameter::location() const {
  string line_number;
  isam::utilities::To<unsigned, string>(line_number_, line_number);
  return string("At line " + line_number + " of file " + file_name_);

}

} /* namespace parameterlist */
} /* namespace isam */
