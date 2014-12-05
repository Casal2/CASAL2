/**
 * @file EmptyModel.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 9/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include "EmptyModel.h"

#include "Model/Model.h"
#include "Utilities/RandomNumberGenerator.h"

// namespaces
namespace niwa {
namespace testfixtures {


EmptyModel::EmptyModel() { }
EmptyModel::~EmptyModel() {}

/**
 *
 */
void EmptyModel::SetUp() {
  Model::Instance(true);

  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  rng.Reset(2468);
}


} /* namespace testfixtures */
} /* namespace niwa */
#endif
