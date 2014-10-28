/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Manager.h"

// Namespaces
namespace isam {
namespace penalties {

/**
 * Default Constructor
 */
Manager::Manager() {
}

/**
 *
 */
PenaltyPtr Manager::GetPenalty(const string& label) const {
  PenaltyPtr result;

  for (PenaltyPtr penalty : objects_) {
    if (penalty->label() == label) {
      result = penalty;
      break;
    }
  }

  return result;
}

/**
 * Get a pointer to a "process" type penalty.
 *
 * @param label The label of the penalty we want to find
 * @return shared_ptr to penalty or empty shared_ptr if not found
 */
penalties::ProcessPtr Manager::GetProcessPenalty(const string& label) {
  for (PenaltyPtr penalty : objects_) {
    if (penalty->type() == PARAM_PROCESS && penalty->label() == label)
      return boost::dynamic_pointer_cast<penalties::Process>(penalty);
  }

  return penalties::ProcessPtr();
}



/**
 * Flag a penalty and store it for retrieval later
 *
 * @param label of the penalty to flag
 * @param value The penalty amount
 */
void Manager::FlagPenalty(const string& label, Double value) {
  Info penalty;
  penalty.label_  = label;
  penalty.score_  = value;

  flagged_penalties_.push_back(penalty);
}

} /* namespace penalties */
} /* namespace isam */
