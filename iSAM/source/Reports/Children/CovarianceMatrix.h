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

#include <boost/numeric/ublas/matrix.hpp>

// namespaces
namespace niwa {
class Minimiser;

namespace reports {
namespace ublas = boost::numeric::ublas;
/**
 *
 */
class CovarianceMatrix : public niwa::Report {
public:
  CovarianceMatrix(Model* model);
  virtual                     ~CovarianceMatrix() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final;
  void                        DoExecute() override final;

private:
  Minimiser*                  minimiser_;
  ublas::matrix<Double>       covariance_matrix_;
  ublas::matrix<Double>       correlation_matrix_;
  double**                    hessian_;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_COVARIANCERMATRIX_H_ */
