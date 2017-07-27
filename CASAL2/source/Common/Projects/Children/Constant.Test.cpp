/**
 * @file Constant.Test.cpp
 * @author C.Marsh
 * @date 21/6/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 */
#ifdef TESTMODE

// Headers
#include "Constant.h"

#include <iostream>

#include "Common/ObjectiveFunction/ObjectiveFunction.h"
#include "Common/Projects/Manager.h"
#include "Common/DerivedQuantities/Manager.h"
#include "Common/Model/Model.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TestResources/Models/CasalComplex1.h"

// Namespaces
namespace niwa {
namespace projects {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;

/*
 * Input projection
*/
const string constant_ycs =
R"(
@project future_ycs
type constant
parameter process[recruitment].ycs_values
years 1999:2010
values 0.478482 0.640663 0.640091 0.762361 0.560125 0.651637 0.764833 0.645498 0.678341 1.234 1.0423 1.4352
)";

/**
 * Run test
 */
TEST_F(InternalEmptyModel, Projects_Constant_YCS) {
	AddConfigurationLine(testresources::models::test_cases_models_casal_complex_1, "TestResources/Models/CasalComplex1.h", 28);
	AddConfigurationLine(constant_ycs, __FILE__, 35);
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
	vector<double> expected = {0.478482, 0.640663, 0.640091, 0.762361, 0.560125, 0.651637, 0.764833, 0.645498, 0.678341, 1.234, 1.0423, 1.4352};
	unsigned iter = 0;
	for (auto value : values) {
		EXPECT_DOUBLE_EQ(expected[iter], value.second);
		++iter;
	}
	// test the final SSB's haven't changed, this will ensure as well as saving the parameters the parameters in the underlying system have changed as well
  vector<double> Expect = {14368038.374395384,14928440.527171403,15070144.769564373,14777935.080570009,14158764.733015139,13414971.755844019,12691104.066415839,12072250.878944062,11631062.235041533,11385845.658086125};
	for (unsigned i = 0; i < 10; ++i) {
    unsigned year = 2002 + i;
    EXPECT_DOUBLE_EQ(Expect[i], dq->GetValue(year)) << " for year " << year << " and value " << Expect[i];
  }
}

/**
 *	Throw an exception as not projected ycs block
 */
TEST_F(InternalEmptyModel, Projects_Throw_Exception) {
	AddConfigurationLine(testresources::models::test_cases_models_casal_complex_1, "TestResources/Models/CasalComplex1.h", 28);
	LoadConfiguration();
	EXPECT_THROW(model_->Start(RunMode::kProjection), std::string);
}

const string future_catch =
R"(
@project future_catch
type constant
parameter process[fishing].catches
years 2003:2012
values 4000
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Projects_Constant_Catches) {
	AddConfigurationLine(testresources::models::test_cases_models_casal_complex_1, "TestResources/Models/CasalComplex1.h", 28);
	AddConfigurationLine(constant_ycs, __FILE__, 35);
	AddConfigurationLine(future_catch, __FILE__, 35);

	LoadConfiguration();
	model_->Start(RunMode::kProjection);

	Project* project = model_->managers().project()->GetProject("future_catch");
	if(!project)
		LOG_FATAL() << "!project";

  DerivedQuantity* dq = model_->managers().derived_quantity()->GetDerivedQuantity("ssb");
	if(!dq)
		LOG_FATAL() << "!dq";

	// test the values have changed
	map<unsigned,Double>& values = project->projected_parameters();
	unsigned iter = 0;
	for (auto value : values) {
		EXPECT_DOUBLE_EQ(4000, value.second);
		++iter;
	}
	// test the final SSB's haven't changed, this will ensure as well as saving the parameters the parameters in the underlying system have changed as well
  vector<double> Expect = {14368038.374395384,14924973.940069387,15063753.632226624,14769108.925828975,14147929.141224194,13402490.288907949,12677276.8995887,12057321.366747757,11615224.601094469,11369265.907976521};
	for (unsigned i = 0; i < 10; ++i) {
    unsigned year = 2002 + i;
    EXPECT_DOUBLE_EQ(Expect[i], dq->GetValue(year)) << " for year " << year << " and value " << Expect[i];
  }
}
} /* namespace projects */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
