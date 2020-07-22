/**
 * @file EmpiricalSampling.Test.cpp
 * @author C.Marsh
 * @date 21/6/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 */
#ifdef TESTMODE

// Headers
#include "EmpiricalSampling.h"

#include <iostream>

#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Projects/Manager.h"
#include "DerivedQuantities/Manager.h"
#include "Model/Model.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TestResources/Models/CasalComplex1.h"
#include "Utilities/RandomNumberGenerator.h"

// Namespaces
namespace niwa {
namespace projects {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;


const string EmpiricalSampling_ycs =
R"(
@project future_ycs
type empirical_sampling
parameter process[recruitment].ycs_values
years 1999:2010
start_year 1974
final_year 1999
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Projects_Empirical_Sampling_YCS) {

	AddConfigurationLine(testresources::models::test_cases_models_casal_complex_1, "TestResources/Models/CasalComplex1.h", 28);
	AddConfigurationLine(EmpiricalSampling_ycs, __FILE__, 35);
	LoadConfiguration();

	model_->Start(RunMode::kProjection);


	Project* project = model_->managers().project()->GetProject("future_ycs");
	if(!project)
		LOG_FATAL() << "!project";

    DerivedQuantity* dq = model_->managers().derived_quantity()->GetDerivedQuantity("ssb");
	if(!dq)
		LOG_FATAL() << "!dq";

	// test the values have changed
	map<unsigned,Double>& values = project->projected_parameters();
	vector<double> expected = {1.532, 1.325, 0.9340, 1.325, 1.014, 0.9320, 1.0487, 0.3520, 1.345, 1.5434, 0.946, 1.0487};
	unsigned iter = 0;
	for (auto value : values) {
		EXPECT_DOUBLE_EQ(expected[iter], value.second);
		++iter;
	}
    // test the final SSB's haven't changed, this will ensure as well as saving the parameters the parameters in the underlying system have changed as well
    vector<double> Expect = {14486323.65, 15356323.90, 16126226.23, 16578541.75, 16610592.93, 16494639.25, 16287801.68, 15815530.62, 15296727.47, 14815818.21};
    for (unsigned i = 0; i < 10; ++i) {
      unsigned year = 2002 + i;
      EXPECT_NEAR(Expect[i], dq->GetValue(year), 1e-2);
    }
}


} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
