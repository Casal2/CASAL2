/*
 * CovarianceMatrix.h
 *
 *  Created on: 10.9.2015
 *      Author: Admin
 */

#ifndef REPORTS_COVARIANCERMATRIX_H_
#define REPORTS_COVARIANCERMATRIX_H_

// headers
#include <boost/numeric/ublas/matrix.hpp>

#include "../../Reports/Report.h"

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
  CovarianceMatrix();
  virtual ~CovarianceMatrix() = default;
  void DoValidate(shared_ptr<Model> model) final{};
  void DoBuild(shared_ptr<Model> model) final{};
  void DoExecute(shared_ptr<Model> model) final;
  void DoPrepareTabular(shared_ptr<Model> model) final;

private:
  ublas::matrix<double> covariance_matrix_;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_COVARIANCERMATRIX_H_ */
