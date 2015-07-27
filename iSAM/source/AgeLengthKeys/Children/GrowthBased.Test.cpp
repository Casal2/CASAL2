//============================================================================
// Name        : GrowthBased.Test.cpp
// Copyright   : Copyright NIWA Science ©2009 - www.niwa.co.nz
//============================================================================
#ifdef TESTMODE

// Headers
#include "GrowthBased.h"

#include <iostream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "AgeLengthKeys/Children/GrowthBased.h"
#include "Utilities/RandomNumberGenerator.h"

// Namespaces
namespace niwa {
namespace agelengthkeys {

using std::cout;
using std::endl;

// classes
class GrowthBasedTest {
public:
  void CummulativeNormal(Double mu, Double cv, vector<Double> *vprop_in_length, vector<Double> class_mins, string distribution, vector<Double>  Class_min_temp, bool plus_grp) {
    GrowthBased x;
    x.CummulativeNormal(mu, cv, vprop_in_length, class_mins, distribution, Class_min_temp, plus_grp);
  }
};

/**
 * Test
 */
TEST(AgeLengthKeys, CummulativeNormal_1) {
	Double          mu = 35.49858;
	Double          cv = 3.549858;
	vector<Double>  vprop_in_length;
	vector<Double>  class_mins = {0, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 ,32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};
	string          distribution = "normal";
	vector<Double>  Class_min_temp = {};
	bool 			      plus_grp = 1;

	GrowthBasedTest myTest;
	myTest.CummulativeNormal(mu, cv, &vprop_in_length, class_mins, distribution, Class_min_temp, plus_grp);

 vector<Double> expected = {3.871360e-09, 1.596024e-08, 7.422371e-08, 3.190201e-07, 1.267264e-06, 4.652548e-06, 1.578663e-05, 4.950653e-05, 1.434857e-04, 3.843497e-04,9.515104e-04,
 2.177043e-03, 4.603455e-03, 8.996259e-03, 1.624797e-02, 2.712032e-02, 4.183597e-02, 5.964394e-02, 7.858619e-02, 9.569515e-02,
 1.076949e-01, 1.120122e-01, 1.076708e-01, 9.565224e-02, 7.853334e-02, 5.959046e-02, 4.178909e-02, 2.708385e-02, 1.622248e-02, 8.980133e-03,
 4.594173e-03, 2.172166e-03, 9.491660e-04, 5.977795e-04};

 ASSERT_EQ(expected.size(), vprop_in_length.size());
 for (unsigned i = 0; i < expected.size(); ++i) {
   EXPECT_EQ(expected[i], vprop_in_length[i]) << " with i = " << i;
 }
}

} /* namespace agelengthkeys */
} /* namespace niwa */

#endif
