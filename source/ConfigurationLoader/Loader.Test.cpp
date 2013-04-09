/**
 * @file Loader.Test.cpp
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
#include "Loader.h"

#include <iostream>

#include "Model/Model.h"
#include "Processes/Manager.h"
#include "TestResources/TestFixtures/EmptyModel.h"

// Namespaces
namespace isam {
namespace configuration {

using std::cout;
using std::endl;
using isam::testfixtures::EmptyModel;

/**
 *
 */
TEST_F(EmptyModel, Configuration_Loader_Test_1_Comments) {

  Loader new_loader;
  new_loader.LoadConfigFile("testcases\\configuration_loader.Test.1");

  ModelPtr model = Model::Instance();
  model->Start();

  // Check the model parameters are right
  EXPECT_EQ(1994u, model->start_year());
  EXPECT_EQ(2008u, model->final_year());
  EXPECT_EQ(1u, model->min_age());
  EXPECT_EQ(50u, model->max_age());
  EXPECT_EQ(true, model->age_plus());

  isam::ProcessPtr process = processes::Manager::Instance().GetProcess("my_recruitment");
  vector<string> categories = process->parameters().Get(PARAM_CATEGORIES).GetValues<string>();
  ASSERT_TRUE(categories.size() != 0);
  EXPECT_EQ(2u, categories.size());
  EXPECT_EQ("immature.male", categories[0]);
  EXPECT_EQ("immature.female", categories[1]);
}

} /* namespace configuration */
} /* namespace isam */


#endif /* TESTMODE */
