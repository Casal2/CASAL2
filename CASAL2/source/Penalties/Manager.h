/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/02/2013
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
#ifndef PENALTIES_MANAGER_H_
#define PENALTIES_MANAGER_H_

// Headers
#include "../BaseClasses/Manager.h"
#include "../Penalties/Common/Process.h"
#include "../Penalties/Penalty.h"
#include "../Utilities/Types.h"

// Namespaces
namespace niwa {
namespace penalties {

using niwa::utilities::Double;

/**
 * Struct Definition
 */
struct Info {
  string label_;
  Double score_;
};

/**
 * Class Definition
 */
class Manager : public niwa::base::Manager<penalties::Manager, niwa::Penalty> {
  friend class niwa::base::Manager<penalties::Manager, niwa::Penalty>;
  friend class niwa::Managers;

public:
  // Methods
  virtual ~Manager() noexcept(true){};
  Penalty*            GetPenalty(const string& label) const;
  penalties::Process* GetProcessPenalty(const string& label);

  // Accessors

protected:
  // methods
  Manager();

private:
  // Members

};

} /* namespace penalties */
} /* namespace niwa */
#endif /* PENALTIES_MANAGER_H_ */
