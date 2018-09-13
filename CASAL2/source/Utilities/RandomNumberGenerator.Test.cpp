/**
 *
 */
#ifdef TESTMODE

// headers
#include "RandomNumberGenerator.h"

#include <iostream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// namespaces
namespace niwa {
namespace utilities {

using std::cout;
using std::endl;


TEST(RandomNumberGenerator, Reset) {

  RandomNumberGenerator& rng = RandomNumberGenerator::Instance();

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(0.45965513936243951, rng.uniform());
  EXPECT_DOUBLE_EQ(0.13950349437072873,  rng.uniform());
  EXPECT_DOUBLE_EQ(0.51783327478915453,   rng.uniform());
  EXPECT_DOUBLE_EQ(0.12138748820871115,  rng.uniform());
  EXPECT_DOUBLE_EQ(0.48201335011981428,  rng.uniform());

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(0.45965513936243951, rng.uniform());
  EXPECT_DOUBLE_EQ(0.13950349437072873,  rng.uniform());
  EXPECT_DOUBLE_EQ(0.51783327478915453,   rng.uniform());
  EXPECT_DOUBLE_EQ(0.12138748820871115,  rng.uniform());
  EXPECT_DOUBLE_EQ(0.48201335011981428,  rng.uniform());

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(3.9456252547909259,  rng.gamma(3.9));
  EXPECT_DOUBLE_EQ(1.5001021695211758,  rng.gamma(2));
  EXPECT_DOUBLE_EQ(0.6314530433244121,  rng.gamma(2));
  EXPECT_DOUBLE_EQ(0.60625575526097708,  rng.gamma(2));
  EXPECT_DOUBLE_EQ(1.2348448618569483,  rng.gamma(2));

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(1.1669253958061021,  rng.normal(1.0));
  EXPECT_DOUBLE_EQ(0.35450280089513919,  rng.normal(1.5));
  EXPECT_DOUBLE_EQ(3.8388838387929232,  rng.normal(2.0));
  EXPECT_DOUBLE_EQ(2.9922527506646963,  rng.normal(2.5));
  EXPECT_DOUBLE_EQ(2.2004228359824394,  rng.normal(3.0));
  EXPECT_DOUBLE_EQ(2.3268201460069662,  rng.normal(3.5));

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(1.8346269790305108,  rng.normal(1.0, 5.0));
  EXPECT_DOUBLE_EQ(-3.0819887964194432,  rng.normal(1.5, 4.0));
  EXPECT_DOUBLE_EQ(7.5166515163787704,  rng.normal(2.0, 3.0));
  EXPECT_DOUBLE_EQ(3.4845055013293926,  rng.normal(2.5, 2.0));
  EXPECT_DOUBLE_EQ(1.6007399629692693,  rng.normal(3.0, 1.75));
  EXPECT_DOUBLE_EQ(1.7402302190104493,  rng.normal(3.5, 1.50));

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(2.1669253958061021,  rng.normal(2.0));
  EXPECT_DOUBLE_EQ(0.85450280089513919,  rng.normal(2.0));
  EXPECT_DOUBLE_EQ(3.8388838387929232,  rng.normal(2.0));
  EXPECT_DOUBLE_EQ(2.4922527506646963,  rng.normal(2.0));
  EXPECT_DOUBLE_EQ(1.2004228359824396,  rng.normal(2.0));
  EXPECT_DOUBLE_EQ(0.82682014600696618,  rng.normal(2.0));

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(0.26507443074399145,   rng.lognormal(1.0, 5.0));
  EXPECT_DOUBLE_EQ(0.052904872387352184,  rng.lognormal(1.5, 4.0));
  EXPECT_DOUBLE_EQ(10.300854560524266,    rng.lognormal(2.0, 3.0));
  EXPECT_DOUBLE_EQ(2.0876968129903184,    rng.lognormal(2.5, 2.0));
  EXPECT_DOUBLE_EQ(0.57754613759328555,   rng.lognormal(3.0, 1.75));
  EXPECT_DOUBLE_EQ(0.54322181469125042,   rng.lognormal(3.5, 1.50));

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(0.81477095523774223,    rng.lognormal(2.0, 3.0));
  EXPECT_DOUBLE_EQ(0.11120789599986763,    rng.lognormal(2.0, 3.0));
  EXPECT_DOUBLE_EQ(10.300854560524266,     rng.lognormal(2.0, 3.0));
  EXPECT_DOUBLE_EQ(1.3348411719281721,     rng.lognormal(2.0, 3.0));
  EXPECT_DOUBLE_EQ(0.18797516110665299,    rng.lognormal(2.0, 3.0));
  EXPECT_DOUBLE_EQ(0.10663320714997929,    rng.lognormal(2.0, 3.0));

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(5.0,    rng.binomial(0.5, 10.0));
  EXPECT_DOUBLE_EQ(3.0,    rng.binomial(0.5, 10.0));
  EXPECT_DOUBLE_EQ(5.0,    rng.binomial(0.5, 10.0));
  EXPECT_DOUBLE_EQ(5.0,    rng.binomial(0.5, 10.0));
  EXPECT_DOUBLE_EQ(4.0,    rng.binomial(0.5, 10.0));

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(2.1669253958061021,   rng.normal(2.0));
  EXPECT_DOUBLE_EQ(0.85450280089513919,  rng.normal(2.0));
  EXPECT_DOUBLE_EQ(3.8388838387929232,   rng.normal(2.0));
  EXPECT_DOUBLE_EQ(2.4922527506646963,   rng.normal(2.0));
  EXPECT_DOUBLE_EQ(1.2004228359824396,   rng.normal(2.0));
  EXPECT_DOUBLE_EQ(0.82682014600696618,  rng.normal(2.0));

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(1.3400279209221049,   rng.chi_square(2.0));
  EXPECT_DOUBLE_EQ(3.6238065431107578,   rng.chi_square(2.0));
  EXPECT_DOUBLE_EQ(2.0156746110334809,   rng.chi_square(2.0));
  EXPECT_DOUBLE_EQ(0.75105696447492076,  rng.chi_square(2.0));
  EXPECT_DOUBLE_EQ(2.4617954154920803,   rng.chi_square(2.0));
  EXPECT_DOUBLE_EQ(0.70000371866995148,  rng.chi_square(2.0));
}


} /* namespace utilities */
} /* namespace niwa */


#endif
