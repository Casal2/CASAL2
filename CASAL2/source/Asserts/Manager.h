/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 19/08/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef ASSERTS_MANAGER_H_
#define ASSERTS_MANAGER_H_

// Headers
#include "../Asserts/Assert.h"
#include "../BaseClasses/Manager.h"
#include "../Model/Managers.h"

// Namespaces
namespace niwa {
namespace asserts {

/**
 * Class Definition
 */
class Manager : public niwa::base::Manager<niwa::asserts::Manager, niwa::Assert> {
  friend class niwa::base::Manager<niwa::asserts::Manager, niwa::Assert>;
  friend class niwa::Managers;

public:
  virtual ~Manager() = default;

protected:
  // methods
  Manager() = default;
};

} /* namespace asserts */
} /* namespace niwa */
#endif /* ASSERTS_MANAGER_H_ */
