/**
 * @file LogNormal.Test.cpp
 * @author C.Marsh
 * @date 21/6/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 */
#ifdef TESTMODE

// Headers
#include "LogNormal.h"

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


const string LogNormal_ycs =
R"(
@project future_ycs
type lognormal
parameter process[recruitment].ycs_values
years 1999:2010
sigma 1.2
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Projects_LogNormal_YCS) {

	AddConfigurationLine(testresources::models::test_cases_models_casal_complex_1, "TestResources/Models/CasalComplex1.h", 28);
	AddConfigurationLine(LogNormal_ycs, __FILE__, 35);
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
	vector<double> expected = {0.59470516037518018, 0.1231198954127662, 4.4223030240241812, 0.87871321335550301, 0.18646856678311435, 0.11909713899382668, 1.0829707265041799, 0.25036818220616419, 0.10853466523372596, 0.15612943666532977, 0.83282165103240524, 0.20840475255359467};
	unsigned iter = 0;
	for (auto value : values) {
		EXPECT_DOUBLE_EQ(expected[iter], value.second);
		++iter;
	}
	// test the final SSB's haven't changed, this will ensure as well as saving the parameters the parameters in the underlying system have changed as well
  vector<double> Expect = {14368038.374395384,14928440.527171403,15843963.138441166,17166238.559592798,19342032.77470737,20140115.411559805,18729074.826858442,16606700.875586545,14904441.060038339,13328985.109193923};
	for (unsigned i = 0; i < 10; ++i) {
    unsigned year = 2002 + i;
    EXPECT_DOUBLE_EQ(Expect[i], dq->GetValue(year)) << " for year " << year << " and value " << Expect[i];
  }

}


} /* namespace projects */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
