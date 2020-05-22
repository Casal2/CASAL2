/**
 * @file EstimateValue.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 15/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will print out the estimate values in a single line separated by spaces
 */
#ifndef ESTIMATEVALUE_H_
#define ESTIMATEVALUE_H_

// headers
#include "Reports/Report.h"

#include <boost/numeric/ublas/matrix.hpp>

// namespaces
namespace niwa {
class Minimiser;

namespace reports {
namespace ublas = boost::numeric::ublas;

/**
 * Class Definition
 */
class EstimateValue : public niwa::Report {
public:
  // Methods
  EstimateValue(Model* model);
  virtual                     ~EstimateValue() noexcept(true);
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final;
  void                        DoFinaliseTabular() override final;

private:
  // members
  bool                        first_run_ = true;
  ublas::matrix<double>       covariance_matrix_;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* ESTIMATEVALUE_H_ */
