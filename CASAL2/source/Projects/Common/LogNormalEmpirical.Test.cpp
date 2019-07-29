/**
 * @file LogNormalEmpirical.Test.cpp
 * @author C.Marsh
 * @date 21/6/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 */
#ifdef TESTMODE

// Headers
#include "LogNormalEmpirical.h"

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


const string LogNormalEmpirical_ycs =
R"(
@project future_ycs
type lognormal_empirical
parameter process[recruitment].ycs_values
years 1999:2010
start_year 1974
final_year 1999
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Projects_LogNormal_Empirical_YCS) {

	AddConfigurationLine(testresources::models::test_cases_models_casal_complex_1, "TestResources/Models/CasalComplex1.h", 28);
	AddConfigurationLine(LogNormalEmpirical_ycs, __FILE__, 35);
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
	vector<double> expected = {1.193159531211873, 0.76433685650904026, 0.59278375352171586, 0.66209045366492325, 1.1015707349908905, 0.72286526934850714, 0.9861550378649957, 0.58579237843666654, 0.80899806415388953, 0.83630043479857519, 0.76944498598285171, 0.72614647686888445};
	unsigned iter = 0;
	for (auto value : values) {
		EXPECT_DOUBLE_EQ(expected[iter], value.second);
		++iter;
	}
	// test the final SSB's haven't changed, this will ensure as well as saving the parameters the parameters in the underlying system have changed as well
  vector<double> Expect = {15112190.605040472,15724504.276644826,15882921.319337627,15392091.662398864,14587729.092482634,13867540.848839389,13515726.024550743,13252531.182031905,12969554.128736978,12614842.753095761};
	for (unsigned i = 0; i < 10; ++i) {
    unsigned year = 2002 + i;
    EXPECT_DOUBLE_EQ(Expect[i], dq->GetValue(year)) << " for year " << year << " and value " << Expect[i];
  }

}


} /* namespace projects */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
