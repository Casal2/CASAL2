/**
 *
 */
#ifdef TESTMODE

// headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

#include "RandomNumberGenerator.h"

// namespaces
namespace niwa {
namespace utilities {

using std::cout;
using std::endl;

/*
 * most of these will be robust as they come from the boost library, more testing if we change random number generator
 * in the future.
 *
 * Note: These are included as we've had issues with boost versions changing the random number sequence previously.
 * These tests ensure model runs from previous Casal2 releases will match the current
 */
TEST(RandomNumberGenerator, Reset) {
  RandomNumberGenerator& rng = RandomNumberGenerator::Instance();

  rng.Reset(2468);
  // This can stay to validate the seed going forward
  EXPECT_DOUBLE_EQ(0.45965513936243951, rng.uniform());
  EXPECT_DOUBLE_EQ(0.13950349437072873, rng.uniform());
  EXPECT_DOUBLE_EQ(0.51783327478915453, rng.uniform());
  EXPECT_DOUBLE_EQ(0.12138748820871115, rng.uniform());
  EXPECT_DOUBLE_EQ(0.48201335011981428, rng.uniform());

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(0.45965513936243951, rng.uniform());
  EXPECT_DOUBLE_EQ(0.13950349437072873, rng.uniform());
  EXPECT_DOUBLE_EQ(0.51783327478915453, rng.uniform());
  EXPECT_DOUBLE_EQ(0.12138748820871115, rng.uniform());
  EXPECT_DOUBLE_EQ(0.48201335011981428, rng.uniform());

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(3.9456252547909259, rng.gamma(3.9));
  EXPECT_DOUBLE_EQ(1.5001021695211758, rng.gamma(2));
  EXPECT_DOUBLE_EQ(0.6314530433244121, rng.gamma(2));
  EXPECT_DOUBLE_EQ(0.60625575526097708, rng.gamma(2));
  EXPECT_DOUBLE_EQ(1.2348448618569483, rng.gamma(2));

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(1.1669253958061021, rng.normal(1.0));
  EXPECT_DOUBLE_EQ(0.35450280089513919, rng.normal(1.5));
  EXPECT_DOUBLE_EQ(3.8388838387929232, rng.normal(2.0));
  EXPECT_DOUBLE_EQ(2.9922527506646963, rng.normal(2.5));
  EXPECT_DOUBLE_EQ(2.2004228359824394, rng.normal(3.0));
  EXPECT_DOUBLE_EQ(2.3268201460069662, rng.normal(3.5));

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(1.8346269790305108, rng.normal(1.0, 5.0));
  EXPECT_DOUBLE_EQ(-3.0819887964194432, rng.normal(1.5, 4.0));
  EXPECT_DOUBLE_EQ(7.5166515163787704, rng.normal(2.0, 3.0));
  EXPECT_DOUBLE_EQ(3.4845055013293926, rng.normal(2.5, 2.0));
  EXPECT_DOUBLE_EQ(1.6007399629692693, rng.normal(3.0, 1.75));
  EXPECT_DOUBLE_EQ(1.7402302190104493, rng.normal(3.5, 1.50));

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(2.1669253958061021, rng.normal(2.0));
  EXPECT_DOUBLE_EQ(0.85450280089513919, rng.normal(2.0));
  EXPECT_DOUBLE_EQ(3.8388838387929232, rng.normal(2.0));
  EXPECT_DOUBLE_EQ(2.4922527506646963, rng.normal(2.0));
  EXPECT_DOUBLE_EQ(1.2004228359824396, rng.normal(2.0));
  EXPECT_DOUBLE_EQ(0.82682014600696618, rng.normal(2.0));

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(0.26507443074399145, rng.lognormal(1.0, 5.0));
  EXPECT_DOUBLE_EQ(0.052904872387352184, rng.lognormal(1.5, 4.0));
  EXPECT_DOUBLE_EQ(10.300854560524266, rng.lognormal(2.0, 3.0));
  EXPECT_DOUBLE_EQ(2.0876968129903184, rng.lognormal(2.5, 2.0));
  EXPECT_DOUBLE_EQ(0.57754613759328555, rng.lognormal(3.0, 1.75));
  EXPECT_DOUBLE_EQ(0.54322181469125042, rng.lognormal(3.5, 1.50));

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(0.81477095523774223, rng.lognormal(2.0, 3.0));
  EXPECT_DOUBLE_EQ(0.11120789599986763, rng.lognormal(2.0, 3.0));
  EXPECT_DOUBLE_EQ(10.300854560524266, rng.lognormal(2.0, 3.0));
  EXPECT_DOUBLE_EQ(1.3348411719281721, rng.lognormal(2.0, 3.0));
  EXPECT_DOUBLE_EQ(0.18797516110665299, rng.lognormal(2.0, 3.0));
  EXPECT_DOUBLE_EQ(0.10663320714997929, rng.lognormal(2.0, 3.0));

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(5.0, rng.binomial(0.5, 10.0));
  EXPECT_DOUBLE_EQ(3.0, rng.binomial(0.5, 10.0));
  EXPECT_DOUBLE_EQ(5.0, rng.binomial(0.5, 10.0));
  EXPECT_DOUBLE_EQ(5.0, rng.binomial(0.5, 10.0));
  EXPECT_DOUBLE_EQ(4.0, rng.binomial(0.5, 10.0));

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(2.1669253958061021, rng.normal(2.0));
  EXPECT_DOUBLE_EQ(0.85450280089513919, rng.normal(2.0));
  EXPECT_DOUBLE_EQ(3.8388838387929232, rng.normal(2.0));
  EXPECT_DOUBLE_EQ(2.4922527506646963, rng.normal(2.0));
  EXPECT_DOUBLE_EQ(1.2004228359824396, rng.normal(2.0));
  EXPECT_DOUBLE_EQ(0.82682014600696618, rng.normal(2.0));

  rng.Reset(2468);
  EXPECT_DOUBLE_EQ(1.3400279209221049, rng.chi_squared(2.0));
  EXPECT_DOUBLE_EQ(3.6238065431107578, rng.chi_squared(2.0));
  EXPECT_DOUBLE_EQ(2.0156746110334809, rng.chi_squared(2.0));
  EXPECT_DOUBLE_EQ(0.75105696447492076, rng.chi_squared(2.0));
  EXPECT_DOUBLE_EQ(2.4617954154920803, rng.chi_squared(2.0));
  EXPECT_DOUBLE_EQ(0.70000371866995148, rng.chi_squared(2.0));
}

/*
 * This test suite is responsible for confirming the distributions used for simulation match the theoretical first and second moments (mean, variance)
 */
TEST(RandomNumberGenerator, uniform_properties) {
  RandomNumberGenerator& rng = RandomNumberGenerator::Instance();
  rng.Reset(2468);
  //------------------------
  // check expectation and variance
  //------------------------
  // uniform
  unsigned            N_sim = 100000;
  std::vector<Double> runiform(N_sim, 0.0);
  Double              total = 0.0;
  Double              Var   = 0.0;
  for (unsigned i = 0; i < N_sim; ++i) {
    runiform[i] = rng.uniform();
    total += runiform[i];
  }
  Double mean = total / (Double)N_sim;
  for (unsigned i = 0; i < N_sim; ++i) Var += (runiform[i] - mean) * (runiform[i] - mean);
  // mean = 1/2 * (1 - 0)
  EXPECT_NEAR(mean, 0.5, 1e-3);
  // var = 1/12 * (1 - 0)
  EXPECT_NEAR(Var / (Double)N_sim, 0.0833333, 1e-2);
}

TEST(RandomNumberGenerator, normal_properties) {
  RandomNumberGenerator& rng = RandomNumberGenerator::Instance();
  rng.Reset(2468);
  // normal
  unsigned            N_sim = 100000;
  std::vector<Double> rnorm(N_sim, 0.0);
  Double              total = 0.0;
  Double              Var   = 0.0;
  for (unsigned i = 0; i < N_sim; ++i) {
    rnorm[i] = rng.normal(1.3, 0.7);
    total += rnorm[i];
  }
  Double mean = total / (Double)N_sim;
  for (unsigned i = 0; i < N_sim; ++i) Var += (rnorm[i] - mean) * (rnorm[i] - mean);

  EXPECT_NEAR(mean, 1.3, 1e-3);
  // var = 0.7 * 0.7
  EXPECT_NEAR(sqrt(Var / (Double)N_sim), 0.7, 1e-2);
}

TEST(RandomNumberGenerator, lognormal_properties) {
  RandomNumberGenerator& rng = RandomNumberGenerator::Instance();
  rng.Reset(2468);
  // lognormal
  unsigned            N_sim = 100000;
  std::vector<Double> rlnorm(N_sim, 0.0);
  Double              total = 0.0;
  Double              Var   = 0.0;
  for (unsigned i = 0; i < N_sim; ++i) {
    rlnorm[i] = rng.lognormal(2.4, 0.2);
    total += rlnorm[i];
  }
  Double mean = total / (Double)N_sim;
  for (unsigned i = 0; i < N_sim; ++i) Var += (rlnorm[i] - mean) * (rlnorm[i] - mean);
  // note the lognormal() is parameterised by the mean, and cv. This is unusual for the fact that
  // usually the lognormal() is parameterised by mu and cv.
  EXPECT_NEAR(mean, 2.4, 1e-3);
  // - the following R-code generates the variance check
  // - sigma = sqrt(log(0.2^2 + 1));
  // - mu = log(2.4) - sigma^2 / 2
  // - var = (exp(sigma*sigma) - 1) * exp(2*mu + sigma*sigma)
  EXPECT_NEAR(Var / (Double)N_sim, 0.23, 1e-2);
}

TEST(RandomNumberGenerator, binomial_properties) {
  RandomNumberGenerator& rng = RandomNumberGenerator::Instance();
  rng.Reset(2468);
  // lognormal
  unsigned            N_sim = 100000;
  std::vector<Double> rbinom(N_sim, 0.0);
  Double              total = 0.0;
  Double              Var   = 0.0;
  for (unsigned i = 0; i < N_sim; ++i) {
    rbinom[i] = rng.binomial(0.2, 200);
    total += rbinom[i];
  }
  Double mean = total / (Double)N_sim;
  for (unsigned i = 0; i < N_sim; ++i) Var += (rbinom[i] - mean) * (rbinom[i] - mean);
  // Discrete distributions I have lowered the tolerance.
  // mean = n*p
  EXPECT_NEAR(mean, 40, 1e-1);
  // var = n*p*(1-p)
  EXPECT_NEAR(Var / (Double)N_sim, 32, 1e-1);
}

TEST(RandomNumberGenerator, gamma_properties) {
  RandomNumberGenerator& rng = RandomNumberGenerator::Instance();
  rng.Reset(2468);
  // lognormal
  unsigned            N_sim = 100000;
  std::vector<Double> rgamma(N_sim, 0.0);
  Double              total = 0.0;
  Double              Var   = 0.0;
  for (unsigned i = 0; i < N_sim; ++i) {
    rgamma[i] = rng.gamma(2.7);
    total += rgamma[i];
  }
  Double mean = total / (Double)N_sim;
  for (unsigned i = 0; i < N_sim; ++i) Var += (rgamma[i] - mean) * (rgamma[i] - mean);
  // mean = shape
  EXPECT_NEAR(mean, 2.7, 1e-2);
  // var = shape
  EXPECT_NEAR(Var / (Double)N_sim, 2.7, 1e-2);
}

TEST(RandomNumberGenerator, chi_squared_properties) {
  RandomNumberGenerator& rng = RandomNumberGenerator::Instance();
  rng.Reset(2468);
  // lognormal
  unsigned            N_sim = 100000;
  std::vector<Double> rchi(N_sim, 0.0);
  Double              total = 0.0;
  Double              Var   = 0.0;
  for (unsigned i = 0; i < N_sim; ++i) {
    rchi[i] = rng.chi_squared(5);
    total += rchi[i];
  }
  Double mean = total / (Double)N_sim;
  for (unsigned i = 0; i < N_sim; ++i) Var += (rchi[i] - mean) * (rchi[i] - mean);
  // mean = shape
  EXPECT_NEAR(mean, 5, 1e-2);
  // var = shape
  EXPECT_NEAR(Var / (Double)N_sim, 10, 0.1);
}

} /* namespace utilities */
} /* namespace niwa */

#endif
