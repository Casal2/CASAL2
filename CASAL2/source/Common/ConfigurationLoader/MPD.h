/**
 * @file MPD.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Feb 9, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_CONFIGURATIONLOADER_MPD_H_
#define SOURCE_CONFIGURATIONLOADER_MPD_H_

// headers
#include <string>
#include <map>

#include "Common/Utilities/Types.h"

// namespaces
namespace niwa {
class Model;
namespace configuration {
using std::string;
using std::map;
using niwa::utilities::Double;

// classes
class MPD {
public:
  MPD() = delete;
  explicit MPD(Model* model) : model_(model) { };
  virtual                     ~MPD() = default;
  bool                        LoadFile(const string& file_name);

private:
  // members
  Model*                    model_;
};

} /* namespace configuration */
} /* namespace niwa */

#endif /* SOURCE_CONFIGURATIONLOADER_MPD_H_ */
