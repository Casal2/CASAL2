/**
 * @file MCMCSample.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_CONFIGURATIONLOADER_MCMCSAMPLE_H_
#define SOURCE_CONFIGURATIONLOADER_MCMCSAMPLE_H_

// headers
#include <string>

// namespaces
namespace niwa {
class Model;

namespace configuration {
using std::string;

// classes
class MCMCSample {
public:
  // methods
  MCMCSample(Model* model);
  virtual                     ~MCMCSample() = default;
  bool                        LoadFile(const string& file_name);

private:
  // members
  Model*                      model_;
};

} /* namespace configuration */
} /* namespace niwa */

#endif /* SOURCE_CONFIGURATIONLOADER_MCMCSAMPLE_H_ */
