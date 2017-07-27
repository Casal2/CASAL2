/*
 * CorrelationMatrix.h
 *
 *  Created on: 28/06/2016
 *      Author: C Marsh
 */

#ifndef REPORTS_CORRELATIONMATRIX_H_
#define REPORTS_CORRELATIONMATRIX_H_

// headers
#include "Common/Reports/Report.h"

#include <boost/numeric/ublas/matrix.hpp>

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
  CorrelationMatrix(Model* model);
  virtual                     ~CorrelationMatrix() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final { };

private:
  ublas::matrix<Double>       correlation_matrix_;

};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_COVARIANCERMATRIX_H_ */
