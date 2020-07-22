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
    vector<double> expected = {1.2026873514345378, 0.74487853110569424, 0.56667440861348606, 0.6382529693098884, 1.1036686305575889, 0.70149159935613259, 0.97978277382410606, 0.55948783853669015, 0.79180210612644342, 0.82058505319333774, 0.75023512598056974, 0.70491761956904408};
	unsigned iter = 0;
	for (auto value : values) {
		EXPECT_DOUBLE_EQ(expected[iter], value.second);
		++iter;
	}

	// test the final SSB's haven't changed, this will ensure as well as saving the parameters the parameters in the underlying system have changed as well
    vector<double> Expect = {14418911.21, 15031687.75, 15236639.02, 14810959.63, 14060268.51, 13385719.94, 13084900.90, 12864447.80, 12612316.15, 12276566.71,  14294453.21,  14524240.72,  14802669.39,  15306948.26,  17112346.68,  18073583.53,  16981920.2, 15163751.16,  13717890.51,  12354198.6};
	for (unsigned i = 0; i < 10; ++i) {
      unsigned year = 2002 + i;
      EXPECT_NEAR(Expect[i], dq->GetValue(year), 1e-2);// << " for year " << year << " and value " << Expect[i];
    }
}


} /* namespace projects */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
