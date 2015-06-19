/**
 * @file Manager.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 12/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Manager for the age length key objects
 */
#ifndef SOURCE_AGELENGTHKEYS_MANAGER_H_
#define SOURCE_AGELENGTHKEYS_MANAGER_H_

// headers
#include "AgeLengthKeys/AgeLengthKey.h"
#include "BaseClasses/Manager.h"

// namespaces
namespace niwa {
namespace agelengthkeys {

/**
 *
 */
class Manager : public niwa::oldbase::Manager<niwa::agelengthkeys::Manager, niwa::AgeLengthKey> {
  friend class niwa::oldbase::Manager<niwa::agelengthkeys::Manager, niwa::AgeLengthKey>;
public:
  // methods
  virtual                     ~Manager() noexcept(true) { };
  AgeLengthKeyPtr             GetAgeLengthKey(const string& label);

protected:
  // methods
  Manager() { };

};
} /* namespace agelengthkeys */
} /* namespace niwa */

#endif /* SOURCE_AGELENGTHKEYS_MANAGER_H_ */
