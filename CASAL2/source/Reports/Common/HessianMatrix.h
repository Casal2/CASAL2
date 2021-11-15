/*
 * HessianMatrix.h
 *
 *  Created on: 10.9.2015
 *      Author: Admin
 */

#ifndef REPORTS_HESSIANMATRIX_H_
#define REPORTS_HESSIANMATRIX_H_

// headers
#include <boost/numeric/ublas/matrix.hpp>

#include "../../Reports/Report.h"

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
  HessianMatrix();
  virtual ~HessianMatrix() = default;
  void DoValidate(shared_ptr<Model> model) final{};
  void DoBuild(shared_ptr<Model> model) final{};
  void DoExecute(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final;

private:
  Minimiser* minimiser_ = nullptr;
  double**   hessian_   = nullptr;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_COVARIANCERMATRIX_H_ */
