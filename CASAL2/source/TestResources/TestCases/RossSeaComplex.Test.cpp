/**
 * @file TwoSexModel.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 23/01/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file contains the CASAL2 version of the two-sex model.
 */
#ifdef TESTMODE

#include "Model/Model.h"
#include "RossSeaComplex.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace testcases {

using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;

/**
 *
 */
TEST_F(InternalEmptyModel, Model_RossSea_BasicRun) {
  AddConfigurationLine(test_cases_models_casal_ross_sea_complex_with_partition_asserts, __FILE__, 30);
  LoadConfiguration();

  EXPECT_NO_THROW(model_->Start(RunMode::kBasic));
}

} /* namespace testcases */
} /* namespace niwa */

#endif
