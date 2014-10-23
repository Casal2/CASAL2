/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 19/08/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef ASSERTS_MANAGER_H_
#define ASSERTS_MANAGER_H_

// Headers
#include "BaseClasses/Manager.h"
#include "Asserts/Assert.h"

// Namespaces
namespace isam {
namespace asserts {

/**
 * Class Definition
 */
class Manager : public isam::oldbase::Manager<isam::asserts::Manager, isam::Assert> {
  friend class isam::oldbase::Manager<isam::asserts::Manager, isam::Assert>;
public:
  virtual                     ~Manager() = default;

protected:
  // methods
  Manager();
};

} /* namespace asserts */
} /* namespace isam */
#endif /* ASSERTS_MANAGER_H_ */
