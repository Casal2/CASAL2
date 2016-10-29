/*
 * LogisticNormal.h
 *
 *  Created on: Oct 26, 2016
 *      Author: Zaita
 */

#ifndef SOURCE_LIKELIHOODS_CHILDREN_LOGISTICNORMAL_H_
#define SOURCE_LIKELIHOODS_CHILDREN_LOGISTICNORMAL_H_

#include "Likelihoods/Likelihood.h"
#include <boost/numeric/ublas/matrix.hpp>

namespace niwa {
namespace likelihoods {
namespace ublas = boost::numeric::ublas;


class LogisticNormal : public niwa::Likelihood {
public:
  // Methods
  LogisticNormal();
  virtual                     ~LogisticNormal();
  void                        DoValidate() override final;
  Double                      AdjustErrorValue(const Double process_error, const Double error_value) override final;
  void                        SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) override final;
  Double                      GetInitialScore(map<unsigned, vector<observations::Comparison> >& comparisons, unsigned year) override final;
  void                        GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) override final;

protected:
  // Estimable parameters
  Double                      sigma_;
  vector<unsigned>            bins_;
  vector<Double>              rho_;
  bool                        arma_;
  unsigned                    n_bins_;
  bool                        sep_by_sex_;
  bool                        sexed_;


  // Covariance containers
  ublas::matrix<Double>       covariance_matrix_;
  ublas::matrix<Double>       covariance_matrix_lt;
  parameters::Table*          covariance_table_ = nullptr;
  // Methods
  void                        calculate_covariance(Double sigma_, vector<Double> rho_, unsigned N_ages, bool sepbysex, bool ARMA, bool sexed);
  vector<Double>              GetRho(vector<Double>& Phi, unsigned nBin, bool ARMA);
  vector<Double>              RecursiveFilter(vector<Double>& ar_coef, unsigned nBins, vector<Double>& initial_vals);
  bool                        DoCholeskyDecmposition();

};

} /* namespace likelihoods */
} /* namespace niwa */
#endif /* SOURCE_LIKELIHOODS_CHILDREN_LOGISTICNORMAL_H_ */
