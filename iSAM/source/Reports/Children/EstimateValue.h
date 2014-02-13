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

// namespaces
namespace isam {
namespace reports {

/**
 * Class Definition
 */
class EstimateValue : public isam::Report {
public:
  // Methods
  EstimateValue();
  virtual                     ~EstimateValue() noexcept(true);
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        Execute() override final;
};

} /* namespace reports */
} /* namespace isam */
#endif /* ESTIMATEVALUE_H_ */
