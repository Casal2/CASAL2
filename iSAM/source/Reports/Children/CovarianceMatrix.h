/*
 * CovarianceMatrix.h
 *
 *  Created on: 10.9.2015
 *      Author: Admin
 */

#ifndef REPORTS_COVARIANCERMATRIX_H_
#define REPORTS_COVARIANCERMATRIX_H_

// headers
#include "Reports/Report.h"

#include "Minimisers/Manager.h"
// namespaces
namespace niwa {
namespace reports {

/**
 *
 */
class CovarianceMatrix : public niwa::Report {
public:
  CovarianceMatrix();
  virtual                     ~CovarianceMatrix() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final;
  void                        DoExecute() override final;

private:
  MinimiserPtr                minimiser_;
  ublas::matrix<Double>       covariance_matrix_;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_COVARIANCERMATRIX_H_ */
