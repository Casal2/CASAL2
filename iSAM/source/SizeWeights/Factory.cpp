/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "SizeWeights/Manager.h"
#include "SizeWeights/Children/Basic.h"
#include "SizeWeights/Children/None.h"

// namespaces
namespace isam {
namespace sizeweights {

/**
 *
 */
SizeWeightPtr Factory::Create(const string& block_type, const string& object_type) {
  SizeWeightPtr size_weight;

  if (block_type == PARAM_SIZE_WEIGHT && object_type == PARAM_NONE) {
    size_weight = SizeWeightPtr(new None());
  } else if (block_type == PARAM_SIZE_WEIGHT && object_type == PARAM_BASIC) {
    size_weight = SizeWeightPtr(new Basic());
  }

  if (size_weight)
    isam::sizeweights::Manager::Instance().AddObject(size_weight);

  return size_weight;
}

} /* namespace sizeweights */
} /* namespace isam */
