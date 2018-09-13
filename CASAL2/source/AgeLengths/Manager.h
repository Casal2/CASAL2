/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifndef AGELENGTHS_MANAGER_H_
#define AGELENGTHS_MANAGER_H_

// headers
#include "AgeLengths/AgeLength.h"
#include "BaseClasses/Manager.h"
#include "Model/Managers.h"

// namespaces
namespace niwa {
namespace agelengths {

// classes
class Manager : public niwa::base::Manager<niwa::agelengths::Manager, niwa::AgeLength> {
  friend class niwa::base::Manager<niwa::agelengths::Manager, niwa::AgeLength>;
  friend class niwa::Managers;
public:
  // methods
  virtual                     ~Manager() noexcept(true) { };
  AgeLength*                  FindAgeLength(const string& label);

protected:
  // methods
  Manager() { };
};

} /* namespace agelengths */
} /* namespace niwa */
#endif /* AGELENGTHS_MANAGER_H_ */
