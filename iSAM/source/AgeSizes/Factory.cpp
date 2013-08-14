/*
 * Factory.cpp
 *
 *  Created on: 24/07/2013
 *      Author: Admin
 */

#include "Factory.h"

#include "AgeSizes/Manager.h"
#include "AgeSizes/AgeSize.h"

namespace isam {
namespace agesizes {

AgeSizePtr Factory::Create(const string& block_type, const string& object_type) {
  AgeSizePtr result;

  if (block_type == PARAM_AGE_SIZE && object_type == PARAM_NONE) {

  }

  if (result)
    isam::agesizes::Manager::Instance().AddObject(result);

  return result;
}

} /* namespace agesizes */
} /* namespace isam */
