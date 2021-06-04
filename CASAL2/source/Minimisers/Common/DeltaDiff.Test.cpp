/**
 * @file GammaDiff.Test.cpp
 * @author Scott Rasmussen (scott@zaita.com)
 * @brief Test minimiser GammaDiff to ensure we get consistent covariance matrixes etc
 * @version 0.1
 * @date 2021-05-01
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifdef TESTMODE
// Headers
#include <boost/algorithm/string/replace.hpp>
#include <iostream>

#include "../../DerivedQuantities/Manager.h"
#include "../../Estimates/Manager.h"
#include "../../MCMCs/MCMC.h"
#include "../../MCMCs/Manager.h"
#include "../../MPD/MPD.Mock.h"
#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../TestResources/MockClasses/Model.h"
#include "../../TestResources/TestCases/TwoSexModel.h"
#include "../../TestResources/TestFixtures/BaseThreaded.h"
#include "../../TestResources/TestFixtures/BasicModel.h"
#include "../Manager.h"
#include "DeltaDiff.h"

// namespaces
namespace niwa::minimisers {

// using
using std::cout;
using std::endl;

// nicer class for displaying outputs
class DeltaDiffModel : public niwa::testfixtures::BaseThreaded {};

/**
 * @brief Test a simple minimisation of the TwoSex model
 *
 */
TEST_F(DeltaDiffModel, Minimise_TwoSex_OneThread) {
  string ammended_definition = testcases::test_cases_two_sex_model_population;
  boost::replace_all(ammended_definition, "numerical_differences", "deltadiff");
  AddConfigurationLine(ammended_definition, __FILE__, 84);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kEstimation));
  auto model = runner_->model();

  auto estimates = model->managers()->estimate()->GetIsEstimated();
  ASSERT_EQ(4u, estimates.size());
  EXPECT_DOUBLE_EQ(1.8286998651090195e-06, estimates[0]->value());
  EXPECT_DOUBLE_EQ(14990296.223743757, estimates[1]->value());
  EXPECT_DOUBLE_EQ(10.136449899380597, estimates[2]->value());
  EXPECT_DOUBLE_EQ(4.8738161211935749, estimates[3]->value());

  auto minimiser = model->managers()->minimiser()->active_minimiser();
  auto cov       = minimiser->covariance_matrix();

  EXPECT_DOUBLE_EQ(1978.5189982688541, model->objective_function().score());

  ASSERT_EQ(4u, cov.size1());
  ASSERT_EQ(4u, cov.size2());

  vector<double> cov_expected
      = {1.0930418378609889629,    -9047092363.2747020721, -0.0021820053707573825764, -0.099269022572991846842, -9047092363.2746982574,  1.3632058614382962278e+20,
         439312266.45533543825,    2672040901.3441257477,  -0.0021820053707570724946, 439312266.45533084869,    0.078336066688112021561, 0.41645773341589825689,
         -0.099269022572998077969, 2672040901.3442206383,  0.41645773341589842342,    3.1180699146237587094};

  for (unsigned i = 0; i < 4; ++i) {
    for (unsigned j = 0; j < 4; ++j) {
      EXPECT_DOUBLE_EQ(cov_expected[(i * 4) + j], cov(i, j));
    }
  }
}

/**
 * @brief Test a simple minimisation of the TwoSex model
 *
 */
TEST_F(DeltaDiffModel, Minimise_TwoSex_FourThreads) {
  string ammended_definition = testcases::test_cases_two_sex_model_population;
  boost::replace_all(ammended_definition, "numerical_differences", "deltadiff");
  boost::replace_all(ammended_definition, "threads 1", "threads 4");
  AddConfigurationLine(ammended_definition, __FILE__, 76);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kEstimation));
  auto model = runner_->model();

  auto estimates = model->managers()->estimate()->GetIsEstimated();
  ASSERT_EQ(4u, estimates.size());
  EXPECT_DOUBLE_EQ(1.8286998651090195e-06, estimates[0]->value());
  EXPECT_DOUBLE_EQ(14990296.223743757, estimates[1]->value());
  EXPECT_DOUBLE_EQ(10.136449899380597, estimates[2]->value());
  EXPECT_DOUBLE_EQ(4.8738161211935749, estimates[3]->value());

  auto minimiser = model->managers()->minimiser()->active_minimiser();
  auto cov       = minimiser->covariance_matrix();

  EXPECT_DOUBLE_EQ(1978.5189982688541, model->objective_function().score());

  ASSERT_EQ(4u, cov.size1());
  ASSERT_EQ(4u, cov.size2());

  vector<double> cov_expected
      = {1.0930418378609889629,    -9047092363.2747020721, -0.0021820053707573825764, -0.099269022572991846842, -9047092363.2746982574,  1.3632058614382962278e+20,
         439312266.45533543825,    2672040901.3441257477,  -0.0021820053707570724946, 439312266.45533084869,    0.078336066688112021561, 0.41645773341589825689,
         -0.099269022572998077969, 2672040901.3442206383,  0.41645773341589842342,    3.1180699146237587094};

  for (unsigned i = 0; i < 4; ++i) {
    for (unsigned j = 0; j < 4; ++j) {
      EXPECT_DOUBLE_EQ(cov_expected[(i * 4) + j], cov(i, j));
    }
  }
}

}  // namespace niwa::minimisers

#endif