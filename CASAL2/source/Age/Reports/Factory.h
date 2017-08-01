/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef AGE_REPORT_FACTORY_H_
#define AGE_REPORT_FACTORY_H_

// Headers
#include <string>

#include "Common/Reports/Report.h"

// Namespaces
namespace niwa {
class Model;
namespace age {
namespace reports {
using std::string;

/**
 * Class definition
 */
class Factory {
public:
  // methods
  static Report* Create(Model* model, const string& object_type, const string& sub_type);

private:
  // methods
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace reports */
} /* namespace age */
} /* namespace niwa */

#endif /* AGE_REPORT_FACTORY_H_ */
