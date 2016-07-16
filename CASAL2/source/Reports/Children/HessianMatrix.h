/*
 * HessianMatrix.h
 *
 *  Created on: 10.9.2015
 *      Author: Admin
 */

#ifndef REPORTS_HESSIANMATRIX_H_
#define REPORTS_HESSIANMATRIX_H_

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
class HessianMatrix : public niwa::Report {
public:
  HessianMatrix(Model* model);
  virtual                     ~HessianMatrix() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final { };

private:
  Minimiser*                  minimiser_;
  double**                    hessian_;

};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_COVARIANCERMATRIX_H_ */
