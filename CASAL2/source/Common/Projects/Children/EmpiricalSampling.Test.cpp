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

#include "Common/ObjectiveFunction/ObjectiveFunction.h"
#include "Common/Projects/Manager.h"
#include "Common/DerivedQuantities/Manager.h"
#include "Common/Model/Model.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TestResources/Models/CasalComplex1.h"
#include "Common/Utilities/RandomNumberGenerator.h"

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
	vector<double> expected = {1.014, 0.94499999999999995, 0.93400000000000005, 0.94499999999999995, 0.93400000000000005, 1.325, 1.0487, 0.35199999999999998, 1.345, 1.5434000000000001, 0.94599999999999995, 1.0487};
	unsigned iter = 0;
	for (auto value : values) {
		EXPECT_DOUBLE_EQ(expected[iter], value.second);
		++iter;
	}
	// test the final SSB's haven't changed, this will ensure as well as saving the parameters the parameters in the underlying system have changed as well
  vector<double> Expect = {15075468.368182484,15649309.464668676,15819762.460147802,15583918.802098621,15216025.504482362,14947874.832287829,14838694.368666304,14810118.208268102,14705639.439621212,14417736.847245166};
	for (unsigned i = 0; i < 10; ++i) {
    unsigned year = 2002 + i;
    EXPECT_DOUBLE_EQ(Expect[i], dq->GetValue(year)) << " for year " << year << " and value " << Expect[i];
  }

}


} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
