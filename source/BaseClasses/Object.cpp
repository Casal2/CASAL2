/**
 * @file BaseObject.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Object.h"

#include "Utilities/Logging/Logging.h"

namespace isam {
namespace base {
/**
 * Default Constructor
 */
Object::Object() {
}

/**
 * Destructor
 */
Object::~Object() {
}

void Object::CheckForRequiredParameter(const string &label) {
  if (!parameters_.IsDefined(label))
    LOG_ERROR("At line " << defined_line_number_ << " in file " << defined_file_name_
        << ": " << block_type_ << " block requires the '" << label << "' parameter");
}

} /* namespace base */
} /* namespace isam */
