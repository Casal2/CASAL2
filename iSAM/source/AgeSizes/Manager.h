/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGESIZES_MANAGER_H_
#define AGESIZES_MANAGER_H_

// headers
#include "AgeSizes/AgeSize.h"
#include "BaseClasses/Manager.h"

// namespaces
namespace niwa {
namespace agesizes {

/**
 * class definition
 */
class Manager : public niwa::oldbase::Manager<niwa::agesizes::Manager, niwa::AgeSize> {
  friend class niwa::oldbase::Manager<niwa::agesizes::Manager, niwa::AgeSize>;
public:
  // methods
  virtual                     ~Manager() noexcept(true) { };
  AgeSizePtr                  GetAgeSize(const string& label);

protected:
  // methods
  Manager() { };
};

} /* namespace agesizes */
} /* namespace niwa */
#endif /* AGESIZES_MANAGER_H_ */
