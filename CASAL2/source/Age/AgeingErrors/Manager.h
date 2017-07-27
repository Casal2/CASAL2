/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef AGEINGERRORS_MANAGER_H_
#define AGEINGERRORS_MANAGER_H_

// Headers
#include "Age/AgeingErrors/AgeingError.h"
#include "Common/BaseClasses/Manager.h"
#include "Common/Model/Managers.h"

// Namespaces
namespace niwa {
namespace ageingerrors {

/**
 * Class Definition
 */
class Manager : public niwa::base::Manager<niwa::ageingerrors::Manager, niwa::AgeingError> {
  friend class niwa::base::Manager<niwa::ageingerrors::Manager, niwa::AgeingError>;
  friend class niwa::Managers;
public:
  // methods
  virtual                     ~Manager() = default;
  AgeingError*                GetAgeingError(const string& label);

protected:
  // methods
  Manager() {};
};

} /* namespace ageingerrors */
} /* namespace niwa */
#endif /* AGEINGERRORS_MANAGER_H_ */
