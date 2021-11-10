/**
 * @file AddressableInputValueLoader.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 20/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science (c))2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The AddressableInputValueLoaderis responsible for loading a text file (csv)
 * of addressable values to do multiple iterations as part of an input-run configuration.
 *
 * Depending on the run mode the addressable values will be used different.
 * For a standard run the model will do N iterations where N is the amount
 * of values specified for the addressables.
 */
#ifndef CONFIGURATION_ADDRESSABLEINPUTVALUESLOADER_H_
#define CONFIGURATION_ADDRESSABLEINPUTVALUESLOADER_H_

// headers
#include <string>

#include "../Model/Model.h"

// namespaces
namespace niwa {
namespace configuration {

using std::string;

// classes
class AddressableInputValueLoader {
public:
  // methods
  AddressableInputValueLoader(shared_ptr<Model> model) : model_(model) {}
  virtual ~AddressableInputValueLoader() = default;
  void LoadValues(const string& file_name);

private:
  // members
  shared_ptr<Model> model_ = nullptr;
};

} /* namespace configuration */
} /* namespace niwa */

#endif /* CONFIGURATION_ADDRESSABLEINPUTVALUESLOADER_H_ */
