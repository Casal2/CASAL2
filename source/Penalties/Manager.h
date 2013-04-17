/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/02/2013
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
#ifndef PENALTIES_MANAGER_H_
#define PENALTIES_MANAGER_H_

// Headers
#include "BaseClasses/Manager.h"
#include "Penalties/Penalty.h"
#include "Utilities/Types.h"

// Namespaces
namespace isam {
namespace penalties {

using isam::utilities::Double;

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
class Manager : public isam::base::Manager<penalties::Manager, isam::Penalty> {
public:
  // Methods
  Manager();
  virtual                     ~Manager() noexcept(true) {};
  PenaltyPtr                  GetPenalty(const string& label) const;
  void                        FlagPenalty(const string& label, double value);
  void                        Reset() override final { flagged_penalties_.clear(); }

  // Accessors
  const vector<Info>&         flagged_penalties() const { return flagged_penalties_; }

private:
  // Members
  vector<Info>                flagged_penalties_;
};

} /* namespace penalties */
} /* namespace isam */
#endif /* PENALTIES_MANAGER_H_ */
