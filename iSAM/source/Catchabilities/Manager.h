/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef CATCHABILITIES_MANAGER_H_
#define CATCHABILITIES_MANAGER_H_

// Headers
#include "BaseClasses/Manager.h"
#include "Catchabilities/Catchability.h"

// Namespaces
namespace niwa {
namespace catchabilities {

/**
 *
 */
class Manager : public niwa::oldbase::Manager<niwa::catchabilities::Manager, niwa::Catchability> {
  friend class niwa::oldbase::Manager<niwa::catchabilities::Manager, niwa::Catchability>;
public:
  // Methods
  virtual                     ~Manager() noexcept(true);
  CatchabilityPtr             GetCatchability(const string& label);

protected:
  // methods
  Manager();
};

// Typedef
typedef boost::shared_ptr<niwa::catchabilities::Manager> CatchabilityManagerPtr;

} /* namespace catchabilities */
} /* namespace niwa */
#endif /* CATCHABILITIES_MANAGER_H_ */
