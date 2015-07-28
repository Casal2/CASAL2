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
      void CummulativeNormal(Double mu, Double cv, vector<Double> *vprop_in_length, vector<Double> class_mins, string distribution, vector<Double> Class_min_temp, bool plus_grp) {
        GrowthBased x;
        x.CummulativeNormal(mu, cv, vprop_in_length, class_mins, distribution, Class_min_temp, plus_grp);
      }
    };

    /**
     * Test
     */
    TEST(AgeLengthKeys, CummulativeNormal_1) {
      Double mu = 35.49858;
      Double cv = 3.549858;
      vector<Double> vprop_in_length;
      vector<Double> class_mins = {0, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 ,32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};
      string distribution = "normal";
      vector<Double> Class_min_temp = {};
      bool plus_grp = 1;

      GrowthBasedTest myTest;
      myTest.CummulativeNormal(mu, cv, &vprop_in_length, class_mins, distribution, Class_min_temp, plus_grp);

      vector<Double> expected = {3.87136e-09, 1.59602e-008, 7.42236e-008, 3.1902e-007, 1.26726e-006, 4.65254e-006, 1.57866e-005, 4.95064e-005, 0.000143486, 0.000384349,0.000951509,
        0.00217704, 0.00460345, 0.00899625, 0.016248, 0.0271203, 0.0418359, 0.0596439, 0.0785861, 0.0956951,
        0.107695, 0.112012, 0.107671, 0.0956523, 0.0785334, 0.0595905, 0.0417891, 0.0270839, 0.0162225, 0.00898015,
        0.00459418, 0.00217217, 0.000949168, 0.000597781};

      ASSERT_EQ(expected.size(), vprop_in_length.size());
      for (unsigned i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(expected[i], vprop_in_length[i]) << " with i = " << i;
      }
    }

    TEST(AgeLengthKeys, CummulativeNormal_2) {
      Double mu = 35.49858;
      Double cv = 3.549858;
      vector<Double> vprop_in_length;
      vector<Double> class_mins = {0, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 ,32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};
      string distribution = "lognormal";
      vector<Double> Class_min_temp = {};
      bool plus_grp = 1;

      GrowthBasedTest myTest;
      myTest.CummulativeNormal(mu, cv, &vprop_in_length, class_mins, distribution, Class_min_temp, plus_grp);

      vector<Double> expected = {0, 9.99201e-016, 1.13909e-013, 6.90781e-012, 2.48631e-010, 5.68087e-009, 8.71919e-008, 9.42695e-007, 7.47451e-006, 4.49824e-005, 0.000211637, 0.000798628, 0.00247155, 0.00639629,0.0140816,
        0.0267735, 0.0445555, 0.0656767, 0.0866652, 0.103353, 0.112342, 0.112157, 0.103555, 0.088979, 0.0715524, 0.0541266, 0.0386965, 0.0262575, 0.0169761, 0.0104946, 0.00622371, 0.0035513,0.00195511, 0.00209553};

      ASSERT_EQ(expected.size(), vprop_in_length.size());
      for (unsigned i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(expected[i], vprop_in_length[i]) << " with i = " << i;
      }
    }




  } /* namespace agelengthkeys */
} /* namespace niwa */

#endif
