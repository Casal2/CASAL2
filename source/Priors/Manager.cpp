/*
 * Manager.cpp
 *
 *  Created on: 6/03/2013
 *      Author: Admin
 */

#include "Manager.h"

namespace isam {
namespace priors {

Manager::Manager() {
  // TODO Auto-generated constructor stub

}

Manager::~Manager() noexcept(true) {
  // TODO Auto-generated destructor stub
}

/**
 *
 */
PriorPtr Manager::GetPrior(const string& label) {
  PriorPtr result;

  for (PriorPtr prior : objects_) {
    if (prior->label() == label) {
      result = prior;
      break;
    }
  }

  return result;
}

} /* namespace priors */
} /* namespace isam */
