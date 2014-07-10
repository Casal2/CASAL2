/**
 * @file String.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "String.h"

// Namespaces
namespace isam {
namespace utilities {
namespace string {


bool is_valid(const string& test_string) {
 if (test_string.length() == 0)
   return false;
 return true;
}

}
} /* namespace utilities */
} /* namespace isam */
