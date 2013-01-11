/**
 * @file All.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 10/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef ALL_H_
#define ALL_H_

// Headers
#include <map>
#include <vector>
#include <string>

#include "Utilities/Types.h"

// Namespaces
namespace isam {
namespace partition {
namespace accessors {

using std::map;
using std::vector;
using std::pair;
using std::string;
using isam::utilities::Double;

/**
 * Class Definition
 */
class All {
public:
  All();
  virtual ~All();
};

} /* namespace accessors */
} /* namespace partition */
} /* namespace isam */
#endif /* ALL_H_ */
