/**
 * @file VonBertalanffy.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 19/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "VonBertalanffy.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>

// Namespaces
namespace niwa {
namespace agelengths {

using ::testing::Return;

class VonBertalanffyTest {
public:
  void CummulativeNormal(Double mu, Double cv, vector<Double>& vprop_in_length, vector<Double> length_bins, string distribution, bool plus_grp) {
    VonBertalanffy x;
    x.CummulativeNormal(mu, cv, vprop_in_length, length_bins, distribution, plus_grp);
  }
  void BuildCV(unsigned year) {
    VonBertalanffy x;
    x.BuildCV(year);
  }
};


/**
 * Test the cumulative normal function that calculates probability of a being in a length bin at a known age
 * This test is for the normal distribution
 */
TEST(AgeLength, VonBertalanffy_CummulativeNormal_1) {
  Double mu = 35.49858;
  Double cv = 0.1;
  vector<Double> vprop_in_length;
  vector<Double> length_bins = {0, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 ,32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};
  string distribution = "normal";
  bool plus_grp = 1;

  VonBertalanffyTest myTest;
  myTest.CummulativeNormal(mu, cv, vprop_in_length, length_bins, distribution,  plus_grp);

  vector<Double> expected = {3.8713535710499514e-009, 1.5960216925847703e-008, 7.422358561104403e-008, 3.1901955588331532e-007, 1.2672619864595447e-006, 4.6525401673491729e-006,
      1.5786604316003761e-005, 4.9506445653380027e-005,0.00014348551812060073, 0.00038434913282614502,0.00095150900849361175, 0.0021770396325317964, 0.0046034492460040877,
      0.0089962477651120976, 0.016247952527619458, 0.027120294871666895, 0.041835938013406682, 0.059643893860880981, 0.078586144395677904, 0.095695106434901311,0.10769489117184539,
      0.11201216057229546,0.10767078378048922,0.095652266453105428,0.078533378916068819, 0.059590504250288112, 0.041789131783027234 , 0.027083887651074501, 0.01622250783484136,
      0.0089801483958439343, 0.0045941822881506722, 0.002172170763177772, 0.00094916847523229819, 0.00059778133048304927};

  ASSERT_EQ(expected.size(), vprop_in_length.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_DOUBLE_EQ(expected[i], vprop_in_length[i]) << " with i = " << i;
  }
}

/**
 * Test the Cumulative normal function when the distribution is specified as "lognormal" with no plus group
 */
TEST(AgeLength, VonBertalanffy_CummulativeNormal_2) {
  Double mu = 35.49858;
  Double cv = 0.1;
  vector<Double> vprop_in_length;
  vector<Double> length_bins = {0, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 ,32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};
  string distribution = "lognormal";
  bool plus_grp = 0;

  VonBertalanffyTest myTest;
  myTest.CummulativeNormal(mu, cv, vprop_in_length, length_bins, distribution,  plus_grp);

  vector<Double> expected = {0, 9.9920072216264089e-016,1.1390888232654106e-013, 6.907807659217724e-012, 2.4863089365112501e-010, 5.6808661108576075e-009, 8.7191919018181352e-008,
      9.4269457673323842e-007, 7.4745056608538363e-006, 4.4982380957292456e-005, 0.00021163731992057677, 0.00079862796125962365, 0.0024715534075264722, 0.0063962867724943751,0.01408161729231916,
      0.026773528172936767, 0.044555539731829574,  0.065676687795628297, 0.086665248035340148,0.1033532304575121, 0.11234199264093081, 0.11215706824927596, 0.10355520403577334,
      0.088978990579101747, 0.071552403002032916, 0.054126600808507175, 0.038696543948670059, 0.026257468168220055, 0.016976053665368585, 0.010494572238876954, 0.0062237099943515117,
      0.0035513001369547048,0.0019551097231892411 };

  ASSERT_EQ(expected.size(), vprop_in_length.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_DOUBLE_EQ(expected[i], vprop_in_length[i]) << " with i = " << i;
  }
}

///**
// * Test the DoAgeLengthConversion() so that we know we are applying the right probabilities to the right part of the partition
// */
//TEST_F(BasicModel, ALK_DoAgeLengthConversion) {
//
//  vector<string> categories   = {"immature.male"};
//
//  niwa::ProcessPtr process = processes::Factory::Create(PARAM_RECRUITMENT, PARAM_CONSTANT);
//  vector<string> proportions  = { "0.6"};
//  process->parameters().Add(PARAM_LABEL, "recruitment", __FILE__, __LINE__);
//  process->parameters().Add(PARAM_TYPE, "constant", __FILE__, __LINE__);
//  process->parameters().Add(PARAM_CATEGORIES, categories, __FILE__, __LINE__);
//  process->parameters().Add(PARAM_PROPORTIONS, proportions, __FILE__, __LINE__);
//  process->parameters().Add(PARAM_R0, "100000", __FILE__, __LINE__);
//  process->parameters().Add(PARAM_AGE, "1", __FILE__, __LINE__);
//
//  niwa::AgeLengthPtr agelength = agelengths::Factory::Create(PARAM_VON_BERTALANFFY, "");
//  agelength->parameters().Add(PARAM_LABEL, "VB", __FILE__, __LINE__);
//  agelength->parameters().Add(PARAM_LINF, "70", __FILE__, __LINE__);
//  agelength->parameters().Add(PARAM_T0, "-6" , __FILE__, __LINE__);
//  agelength->parameters().Add(PARAM_K, "0.034" , __FILE__, __LINE__);
//
//
//  niwa::AgeLengthKeyPtr agelengthkey = agelengthkeys::Factory::Create(PARAM_AGE_LENGTH_KEY, PARAM_GROWTH_BASED);
//  agelengthkey->parameters().Add(PARAM_LABEL, "ALK", __FILE__, __LINE__);
//  agelengthkey->parameters().Add(PARAM_CLASS_MINIMUMS, {"0","15","30","45","55","70"}, __FILE__, __LINE__);
//  agelengthkey->parameters().Add(PARAM_DISTRIBUTION, "normal" , __FILE__, __LINE__);
//
//  niwa::base::ObjectPtr time_step = timesteps::Factory::Create();
//  vector<string> processes    = { "recruitment", "VB", "ALK"};
//  time_step->parameters().Add(PARAM_LABEL, "step_one", __FILE__, __LINE__);
//  time_step->parameters().Add(PARAM_PROCESSES, processes, __FILE__, __LINE__);
//
//  Model::Instance()->Start(RunMode::kTesting);
//
//  partition::Category& immature_male   = Partition::Instance().category("immature.male");
//
//
//  /**
//   * Do 1 iteration of the model then check the categories to see if
//   * the AGELength key was successful
//   */
//  Model::Instance()->FullIteration();
//
//  // Check i = 0
//  EXPECT_DOUBLE_EQ(0, immature_male.data_[0]);
//
//  //Run through ages and length bins to see if conversion correct
//  for (unsigned age = immature_male.min_age_; age < immature_male.max_age_; age++ )
//  {
//   unsigned bin = 2;
//
//      EXPECT_DOUBLE_EQ(60000.0 , immature_male.length_data_[age][bin]) << " where age = " << age << " where class_bin = " << bin;
//
//  }
//
//}

} /* namespace agelength */
} /* namespace niwa */
#endif
