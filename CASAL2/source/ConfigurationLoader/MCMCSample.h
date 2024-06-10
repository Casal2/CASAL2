/**
 * @file MCMCSample.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/11/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_CONFIGURATIONLOADER_MCMCSAMPLE_H_
#define SOURCE_CONFIGURATIONLOADER_MCMCSAMPLE_H_

// headers
#include <memory>
#include <string>

// namespaces
namespace niwa {
class Model;

namespace configuration {

using std::shared_ptr;
using std::string;

// classes
class MCMCSample {
public:
  // methods
  MCMCSample(shared_ptr<Model> model);
  virtual ~MCMCSample() = default;
  bool LoadFile(const string& file_name);

private:
  // members
  shared_ptr<Model> model_;
};

} /* namespace configuration */
} /* namespace niwa */

#endif /* SOURCE_CONFIGURATIONLOADER_MCMCSAMPLE_H_ */
