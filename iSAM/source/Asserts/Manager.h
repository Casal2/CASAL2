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
class Manager : public isam::base::Manager<isam::asserts::Manager, isam::Assert> {
public:
  Manager();
  virtual                     ~Manager() noexcept(true);
  void                        Validate() override final;
};

} /* namespace asserts */
} /* namespace isam */
#endif /* ASSERTS_MANAGER_H_ */
