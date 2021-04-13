/**
 * @file MCMCObjective.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is responsible for loading a covariance matrix and
 * sample count from the MCMC objective report
 */
#ifndef SOURCE_CONFIGURATIONLOADER_MCMCOBJECTIVE_H_
#define SOURCE_CONFIGURATIONLOADER_MCMCOBJECTIVE_H_

// headers
#include <string>
#include <memory>

// namespaces
namespace niwa {
class Model;

namespace configuration {
using std::string;
using std::shared_ptr;

// classes
class MCMCObjective {
public:
  // methods
  MCMCObjective(shared_ptr<Model> model);
  virtual                     ~MCMCObjective() = default;
  bool                        LoadFile(const string& file_name);

private:
  // members
  shared_ptr<Model>                      model_;
};

} /* namespace configuration */
} /* namespace niwa */

#endif /* SOURCE_CONFIGURATIONLOADER_MCMCOBJECTIVE_H_ */
