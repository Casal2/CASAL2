/*
 * Manager.cpp
 *
 *  Created on: 21/12/2012
 *      Author: Admin
 */

#include "Manager.h"

namespace niwa {
namespace selectivities {

Manager::Manager() {
  // TODO Auto-generated constructor stub

}

/**
 *
 */
SelectivityPtr Manager::GetSelectivity(const string& label) {
  SelectivityPtr result;

  for(SelectivityPtr selectivity : objects_) {
    if (selectivity->label() == label) {
      result = selectivity;
      break;
    }
  }

  return result;
}

} /* namespace selectivities */
} /* namespace niwa */
