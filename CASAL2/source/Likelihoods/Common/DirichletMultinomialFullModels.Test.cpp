/**
 * @file DirichletMultinomialFullModels.Test.cpp
 * @author  C.Marsh
 * @date 11/05/2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 * This will load a full model so that we can test other validations
 * that cannot be tested from the unit test in DirichletMultinomial.Test.cpp
 */
#ifdef TESTMODE

// headers
#include "../../TestResources/Models/TwoSexNoEstimates.h"
#include "../../TestResources/TestFixtures/InternalEmptyModel.h"
#include "../../BaseClasses/Object.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"
#include "../../Model/Objects.Mock.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"

#include "../../TestResources/MockClasses/Model.h"
// namespaces
namespace niwa {
namespace likelihoods {
using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;
/**
 * just a nonsence observation with dirichlet multinomial to test validation implementation
 */
const std::string temp_obs_with_likelihood =
    R"(
@likelihood Dirichlet_multi
type dirichlet_multinomial
theta 1
      
@observation PAA_input
type proportions_at_age
likelihood Dirichlet_multi
time_step one
categories mature.male
min_age 3
max_age 15
selectivities FishingSel
years 1992
time_step_proportion 1.0
table obs
1992 0.0186 0.0219 0.0249 0.0390 0.0512 0.0646 0.0422 0.0677 0.0523 0.0687 0.0299 0.0132 0.0515 0.0055 0.0254 0.0199 0.0320 0.0268 0.0394 0.0250 0.0536 0.0346 0.0423 0.0489 0.0304 0.0705
end_table
table error_values
1992 30
end_table
)";
/**
 *
 */
TEST_F(InternalEmptyModel, DirichletMultinomial_expect_fail_theta_needs_transformation) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(temp_obs_with_likelihood, __FILE__, 31);
  LoadConfiguration();
  // this should error saying theta needs a transformation
  EXPECT_THROW(model_->Start(RunMode::kEstimation), std::string); 
}

} /* namespace likelihoods */
} /* namespace niwa */

#endif /* TESTMODE */
