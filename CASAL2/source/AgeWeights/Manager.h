/**
 * @file Manager.h
 * @author  C.Marsh
 * @date 16/11/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */
#ifndef AGEWEIGHTS_MANAGER_H_
#define AGEWEIGHTS_MANAGER_H_

// headers
#include "../AgeWeights/AgeWeight.h"
#include "../BaseClasses/Manager.h"
#include "../Model/Managers.h"

// namespaces
namespace niwa {
namespace ageweights {

// classes
class Manager : public niwa::base::Manager<niwa::ageweights::Manager, niwa::AgeWeight> {
  friend class niwa::base::Manager<niwa::ageweights::Manager, niwa::AgeWeight>;
  friend class niwa::Managers;
public:
  // methods
  virtual                     ~Manager() noexcept(true) { };
  AgeWeight*                  FindAgeWeight(const string& label);

protected:
  // methods
  Manager() { };
};

} /* namespace agelengths */
} /* namespace niwa */
#endif /* AGEWEIGHTS_MANAGER_H_ */
