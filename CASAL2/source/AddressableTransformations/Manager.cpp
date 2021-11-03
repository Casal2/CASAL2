/**
 * @file Manager.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Dec 7, 2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

// namespaces
namespace niwa {
namespace addressableransformations {

/**
 * Validate the estimate transformations
 */
void Manager::Validate() {
  vector<string> parameters_across_blocks;
  for (auto obj : objects_) {
    obj->Validate();
    for(auto param : obj->GetParameterLabels()) {
      if(find(parameters_across_blocks.begin(), parameters_across_blocks.end(), param) != parameters_across_blocks.end()) {
        LOG_ERROR() << "The parameter " << param << " in @addressable_transformation block " << obj->label() << " is already in another @estimate_tranformation block. You cannot have the same parameter in multiple @estimate_transfomration blocks. Can you please review these inputs";
      }
    }
  }
}



/**
 * This method returns a pointer to the AddressableTransformation
 *
 * @param label The label of the AddressableTransformation
 * @return The pointer to the AddressableTransformation
 */
AddressableTransformation* Manager::GetAddressableTransformation(const string& label) {
  for (AddressableTransformation* addressabletransformation : objects_) {
    if (addressabletransformation->label() == label)
      return addressabletransformation;
  }
  return nullptr;
}
} /* namespace addressableransformations */
} /* namespace niwa */
