/**
 * @file DirichletMultinomial.Test.cpp
 * @author  C.Marsh
 * @date 11/05/2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

#include "../../Likelihoods/Factory.h"
#include "../../Observations/Comparison.h"
#include "../../Utilities/RandomNumberGenerator.h"
#include "DirichletMultinomial.h"

// namespaces
namespace niwa {
namespace likelihoods {

using observations::Comparison;
using std::cout;
using std::endl;

// Mock the likelihood so we can set values
class MockDirichletMultinomial : public likelihoods::DirichletMultinomial {
public:
  MockDirichletMultinomial(shared_ptr<Model> model) : likelihoods::DirichletMultinomial(model) {}
  virtual ~MockDirichletMultinomial() = default;

  void set_theta(Double new_theta) {theta_ = new_theta;};
};


/*
# R-code for the following test
#' Return the Pdf for the dirichlet multinomial tried to copy from The thorson paper
#' He deviates from the classic formulation by pulling out (x!) of the denominator from the right hand product
#' and moving it to the left denominator has been validated agains extraDistr::ddirmnom
#' @param: obs vector of observed compositions assumes sum(pi) = 1
#' @param: fitted vector of fiited compositions assumes sum(pi_fitted) = 1 
#' @param: beta variance inflation coefficient
#' @param: n is the total number of samples in the available data (which is restricted to any non-negative real number),
#' @return PDF
#'
ddirichmult = function(obs, beta, n, fitted, log = F) {
  if(sum(pi) != 1)
    stop("pi needs to sum to 1")
  if(sum(fitted) != 1)
    stop("pi_fitted needs to sum to 1")
  val = (lgamma(n + 1) + lgamma(beta)) - (lgamma(n + beta) + sum(lgamma(n*obs + 1))) + sum(lgamma(n*obs + beta * fitted) - lgamma(beta* fitted))
  if(log == F)
    val = exp(val)
  return(val)
}
set.seed(123)
n = 1000
pi = rnorm(10, 50, 10)
pi_fitted = rnorm(10,  50, 10)
pi = pi / sum(pi)
pi_fitted = pi_fitted / sum(pi_fitted)
size = n
x = pi * n
beta = 7
theta = beta / n ## used in casal2 model
ddirichmult(pi, beta = beta, n = sum(x), fitted = pi_fitted, log = T) # only take integers
#[1] -51.41739 ## this is log-likelihood casal2 does negative-log-likelihood
*/

TEST(Likelihood, DirichletMultinomial) {
  utilities::RandomNumberGenerator::Instance().Reset(31373u);

  MockDirichletMultinomial likelihood(nullptr);
  vector<double> observed = {0.08748477, 0.09399358 ,0.12924517 ,0.09991887, 0.10107716, 0.13232631 ,0.10761220 ,0.07360028, 0.08499439, 0.08974727};
  vector<Double> expected = {0.1195, 0.1029, 0.10369, 0.09812, 0.08532, 0.1303, 0.10555, 0.05824, 0.10946, 0.08692};
  Double n = 1000;
  Double theta = 0.007;  // theta = beta / n
  likelihood.set_theta(theta);
  double expected_negative_log_likelihood = 51.41739; // see the above R-code
  map<unsigned, vector<Comparison> > comparison_list;
  for(unsigned i = 0; i < observed.size(); i++) {
    Comparison comparison;
    comparison.age_           = 0;
    comparison.category_      = "A";
    comparison.expected_      = expected[i];
    comparison.score_         = 0.0;
    comparison.observed_      = observed[i];
    comparison.error_value_   = n;
    comparison.process_error_ = 0.0;
    comparison.delta_         = 1e-15;
    comparison_list[0].push_back(comparison);
  }


  double casal2_result = likelihood.GetInitialScore(comparison_list, 0);

  EXPECT_NEAR(casal2_result, 34.8882, 0.0001);

  

  likelihood.GetScores(comparison_list);
  for(unsigned i = 0; i < observed.size(); i++) {
    casal2_result += comparison_list[0][i].score_;
  }

  // Check initial score
  EXPECT_NEAR(casal2_result, expected_negative_log_likelihood, 0.0001);
}
} /* namespace likelihoods */
} /* namespace niwa */

#endif /* TESTMODE */
