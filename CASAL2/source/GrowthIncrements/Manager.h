/**
 * @file Manager.h
 * @author  C.Marsh
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * 
 */
#ifndef GROWTHINCREMENTS_MANAGER_H_
#define GROWTHINCREMENTS_MANAGER_H_

// headers
#include "../BaseClasses/Manager.h"
#include "../GrowthIncrements/GrowthIncrement.h"

// namespaces
namespace niwa {
namespace growthincrements {

/**
 * class definition
 */
class Manager : public niwa::base::Manager<niwa::growthincrements::Manager, niwa::GrowthIncrement> {
  friend class niwa::base::Manager<niwa::growthincrements::Manager, niwa::GrowthIncrement>;
  friend class niwa::Managers;

public:
  // methods
  virtual ~Manager() noexcept(true){};
  GrowthIncrement* GetGrowthIncrement(const string& label) const;

protected:
  // methods
  Manager();
};

} /* namespace growthincrements */
} /* namespace niwa */
#endif /* GROWTHINCREMENTS_MANAGER_H_ */
