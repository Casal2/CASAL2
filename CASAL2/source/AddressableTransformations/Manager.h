/*
 * Manager.h
 *
 *  Created on: Dec 7, 2015
 *      Author: Zaita
 */

#ifndef SOURCE_ESTIMABLETRANSFORMATIONS_MANAGER_H_
#define SOURCE_ESTIMABLETRANSFORMATIONS_MANAGER_H_

// Headers
#include "../BaseClasses/Manager.h"
#include "../Model/Managers.h"
#include "AddressableTransformation.h"

// Namespaces
namespace niwa {
namespace addressabletransformations {

/**
 * Class Definition
 */
class Manager : public niwa::base::Manager<niwa::addressabletransformations::Manager, niwa::AddressableTransformation> {
  friend class niwa::base::Manager<niwa::addressabletransformations::Manager, niwa::AddressableTransformation>;
  friend class niwa::Managers;

public:
  virtual ~Manager() = default;
  void                       Validate() override final;
  AddressableTransformation* GetAddressableTransformation(const string& label);
  void                       PrepareForObjectiveFunction();
  void                       RestoreForObjectiveFunction();

protected:
  // methods
  Manager() = default;
};

} /* namespace addressabletransformations */
} /* namespace niwa */
#endif /* SOURCE_ESTIMABLETRANSFORMATIONS_MANAGER_H_ */
