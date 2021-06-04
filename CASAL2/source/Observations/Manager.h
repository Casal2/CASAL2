/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef OBSERVATIONS_MANAGER_H_
#define OBSERVATIONS_MANAGER_H_

// Headers
#include "BaseClasses/Manager.h"
#include "Observations/Observation.h"

// Namespaces
namespace niwa {
namespace observations {

/**
 * Class Definition
 */
class Manager : public niwa::base::Manager<observations::Manager, niwa::Observation> {
  friend class niwa::base::Manager<observations::Manager, niwa::Observation>;
  friend class niwa::Managers;

public:
  // methods
  virtual ~Manager() noexcept(true);
  //  void                        PreExecute(unsigned year, const string& time_step);
  //  void                        Execute(unsigned year, const string& time_step);
  Observation* GetObservation(const string& label);
  void         CalculateScores();

protected:
  // methods
  Manager();
};

} /* namespace observations */
} /* namespace niwa */
#endif /* OBSERVATIONS_MANAGER_H_ */
