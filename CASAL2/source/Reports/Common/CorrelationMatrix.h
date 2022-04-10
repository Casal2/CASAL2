/*
 * CorrelationMatrix.h
 *
 *  Created on: 28/06/2016
 *      Author: C Marsh
 */

#ifndef REPORTS_CORRELATIONMATRIX_H_
#define REPORTS_CORRELATIONMATRIX_H_

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
class CorrelationMatrix : public niwa::Report {
public:
  CorrelationMatrix();
  virtual ~CorrelationMatrix() = default;
  void DoValidate(shared_ptr<Model> model) final{};
  void DoBuild(shared_ptr<Model> model) final{};
  void DoExecute(shared_ptr<Model> model) final;
  void DoPrepareTabular(shared_ptr<Model> model) final;

private:
  ublas::matrix<double> correlation_matrix_;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_COVARIANCERMATRIX_H_ */
