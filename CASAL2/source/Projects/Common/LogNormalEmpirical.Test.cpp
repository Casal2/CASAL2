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
  vector<double> Expect = {14416960.8,  15029708.91,  15241070.46,  14841433.57,  14129449.26,  13489008.6, 13203140.65,  12994232.07,  12755996.14,  12438217.98,  14294453.21,  14524240.72,  14802669.39,  15306948.26,  17112346.68,  18073583.53,  16981920.2, 15163751.16,  13717890.51,  12354198.6};
	for (unsigned i = 0; i < 10; ++i) {
    unsigned year = 2002 + i;
    EXPECT_NEAR(Expect[i], dq->GetValue(year), 1e-2);// << " for year " << year << " and value " << Expect[i];
  }

}


} /* namespace projects */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
