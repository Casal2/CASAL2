/*
 * Factory.cpp
 *
 *  Created on: 24/07/2013
 *      Author: Admin
 */

#include "Factory.h"

#include "AgeSizes/Manager.h"
#include "AgeSizes/AgeSize.h"
#include "AgeSizes/Children/None.h"
#include "AgeSizes/Children/Schnute.h"
#include "AgeSizes/Children/VonBertalanffy.h"

namespace isam {
namespace agesizes {

AgeSizePtr Factory::Create(const string& block_type, const string& object_type) {
  AgeSizePtr result;

  if (block_type == PARAM_AGE_SIZE && object_type == PARAM_NONE) {
    result = AgeSizePtr(new None());

  } else if (block_type == PARAM_AGE_SIZE && object_type == PARAM_SCHNUTE) {
    result = AgeSizePtr(new Schnute());

  } else if (block_type == PARAM_AGE_SIZE && object_type == PARAM_VON_BERTALANFFY) {
    result = AgeSizePtr(new VonBertalanffy());

  }

  if (result)
    isam::agesizes::Manager::Instance().AddObject(result);

  return result;
}

} /* namespace agesizes */
} /* namespace isam */
