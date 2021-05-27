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
#include <iostream>

#include "../../ConfigurationLoader/MPD.Mock.h"
#include "../../DerivedQuantities/Manager.h"
#include "../../Estimates/Manager.h"
#include "../../MCMCs/MCMC.h"
#include "../../MCMCs/Manager.h"
#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../TestResources/MockClasses/Model.h"
#include "../../TestResources/TestCases/TwoSexModel.h"
#include "../../TestResources/TestFixtures/BasicModel.h"
#include "../../TestResources/TestFixtures/InternalEmptyModel.h"
#include "../Manager.h"
#include "GammaDiff.h"

// namespaces
namespace niwa::minimisers {

// using
using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;

// nicer class for displaying outputs
class GammaDiffModel : public InternalEmptyModel {};

/**
 * @brief Test a simple minimisation of the TwoSex model
 *
 */
TEST_F(GammaDiffModel, Minimise_TwoSex) {
  AddConfigurationLine(testcases::test_cases_two_sex_model_population, __FILE__, 27);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  auto estimates = model_->managers()->estimate()->GetIsEstimated();
  ASSERT_EQ(4u, estimates.size());
  EXPECT_DOUBLE_EQ(1.1069300534419012e-05, estimates[0]->value());
  EXPECT_DOUBLE_EQ(2372190.8435692932, estimates[1]->value());
  EXPECT_DOUBLE_EQ(10.188652722538006, estimates[2]->value());
  EXPECT_DOUBLE_EQ(4.8909677429617107, estimates[3]->value());

  auto minimiser = model_->managers()->minimiser()->active_minimiser();
  auto cov       = minimiser->covariance_matrix();

  ASSERT_EQ(4u, cov.size1());
  ASSERT_EQ(4u, cov.size2());

  vector<double> cov_expected = {2.1393391220528406e-12, -0.32106843155626813, -4.4528884641258918e-09, -9.7477276343483175e-10, -0.32106843155626824, 66990025122.572098,
                                 -519.28024905818529,    -143.3736627167597,   -4.4528884641303478e-09, -519.28024905725636,     0.01231934843875214,  0.010075430463480958,
                                 -9.747727634323713e-10, -143.37366271727373,  0.010075430463480963,    0.012946294921863813};

  for (unsigned i = 0; i < 4; ++i) {
    for (unsigned j = 0; j < 4; ++j) {
      EXPECT_DOUBLE_EQ(cov_expected[(i * 4) + j], cov(i, j));
    }
  }
}

}  // namespace niwa::minimisers

#endif
