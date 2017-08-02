/**
 * @file Factory.h
 * @author  C.Marsh
 * @version 1.0
 * @date 1/08/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */
#ifndef LENGTH_REPORT_FACTORY_H_
#define LENGTH_REPORT_FACTORY_H_

// Headers
#include <string>

#include "Common/Reports/Report.h"

// Namespaces
namespace niwa {
class Model;
namespace length {
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
} /* namespace length */
} /* namespace niwa */
#endif /* LENGTH_REPORT_FACTORY_H_ */
