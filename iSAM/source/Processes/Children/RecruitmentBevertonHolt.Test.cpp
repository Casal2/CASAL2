/**
 * @file RecruitmentBevertonHolt.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 20/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include <iostream>

#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Processes/Manager.h"
#include "Model/Model.h"
#include "Partition/Partition.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TestResources/Models/CasalComplex1.h"

// Namespaces
namespace isam {
namespace processes {

using std::cout;
using std::endl;
using isam::testfixtures::InternalEmptyModel;

/**
 *
 */
TEST_F(InternalEmptyModel, Processes_BevertonHolt_Recruitment) {
  AddConfigurationLine(testresources::models::test_cases_models_casal_complex_1, "TestResources/Models/CasalComplex1.h", 32);
  LoadConfiguration();

  ModelPtr model = Model::Instance();
  model->Start(RunMode::kBasic);

  partition::Category& male   = Partition::Instance().category("male");
  EXPECT_DOUBLE_EQ(0.0, male.data_[0]);
  EXPECT_DOUBLE_EQ(2499927.5474240803, male.data_[1]);
  EXPECT_DOUBLE_EQ(2046756.2758644461, male.data_[2]);

  EXPECT_DOUBLE_EQ(151391.67827268399, male.data_[15]);
  EXPECT_DOUBLE_EQ(123889.51433539754, male.data_[16]);
  EXPECT_DOUBLE_EQ(101388.98722028485, male.data_[17]);

  EXPECT_DOUBLE_EQ(45509.118554179389, male.data_[21]);
  EXPECT_DOUBLE_EQ(37254.617197639338, male.data_[22]);
  EXPECT_DOUBLE_EQ(168174.98846106316, male.data_[23]);

  partition::Category& female = Partition::Instance().category("female");
  EXPECT_DOUBLE_EQ(0.0, male.data_[0]);
  EXPECT_DOUBLE_EQ(2499927.5474240803, female.data_[1]);
  EXPECT_DOUBLE_EQ(2046760.4571484025, female.data_[2]);

  EXPECT_DOUBLE_EQ(151581.19489755956, female.data_[15]);
  EXPECT_DOUBLE_EQ(124062.47306900102, female.data_[16]);
  EXPECT_DOUBLE_EQ(101543.48455296629, female.data_[17]);

  EXPECT_DOUBLE_EQ(45592.839274420774, female.data_[21]);
  EXPECT_DOUBLE_EQ(37324.680701432851, female.data_[22]);
  EXPECT_DOUBLE_EQ(168519.01800652978, female.data_[23]);
}

} /* namespace processes */
} /* namespace isam */


#endif /* TESTMODE */
