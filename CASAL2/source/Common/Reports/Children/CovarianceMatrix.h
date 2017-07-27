/*
 * CovarianceMatrix.h
 *
 *  Created on: 10.9.2015
 *      Author: Admin
 */

#ifndef REPORTS_COVARIANCERMATRIX_H_
#define REPORTS_COVARIANCERMATRIX_H_

// headers
#include "Common/Reports/Report.h"

#include <boost/numeric/ublas/matrix.hpp>

// namespaces
namespace niwa {
class MCMC;
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
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final { };

private:
  ublas::matrix<Double>       covariance_matrix_;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_COVARIANCERMATRIX_H_ */
