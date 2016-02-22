/**
 * @file LogNormal.Test.cpp
 * @author Craig Marsh
 * @github https://github.com/Zaita
 * @date Jan 11, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include "Partition/Partition.h"
#include "Model/Model.h"
#include "TestResources/TestCases/TwoSexModel.h"
#include "TestResources/Models/TwoSex.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

#include "Utilities/RandomNumberGenerator.h"

namespace niwa {
namespace projects {


using niwa::testfixtures::InternalEmptyModel;

const string lognormal_map =
R"(
@project future_catches
type lognormal
parameter process[Fishing].catches(2008:2012)
mean 10.0944
sigma 1
)";

/**
 *  Test LogNormal @project for estimate of type map and
 */

TEST_F(InternalEmptyModel, lognormal_map_projection) {
//  utilities::RandomNumberGenerator::Instance().Reset(3445u);
//  AddConfigurationLine(testresources::models::two_sex, "TestResources/Models/TwoSex.h", 28);
//  AddConfigurationLine(lognormal_map, __FILE__, 22);
//  LoadConfiguration();
//
//  model_->Start(RunMode::kProjection);
//
//  partition::Category& immature_male   = model_->partition().category("immature.male");
//  partition::Category& immature_female = model_->partition().category("immature.female");
//
//  // Verify blank partition for the last time_step
//  for (unsigned i = 0; i < immature_male.data_.size(); ++i)
//    EXPECT_DOUBLE_EQ(0.0, immature_male.data_[i]) << " where i = " << i << "; age = " << i + immature_male.min_age_;
//  for (unsigned i = 0; i < immature_female.data_.size(); ++i)
//    EXPECT_DOUBLE_EQ(0.0, immature_female.data_[i]) << " where i = " << i << "; age = " << i + immature_female.min_age_;
}

const string lognormal_vector =
R"(
@project future_catches
type lognormal
parameter process[Fishing].catches(2008:2012)
mean 10.0944
sigma 1
)";

} /* namespace projects */
} /* namespace niwa */
#endif
