/**
 * @file Partition_YearCrossAgeMatrix.cpp
 * @author  Marco Kienzle (Marco.Kienzle@niwa.co.nz) based on Partition.h by Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 26/08/2018
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will print the partition for the target
 * time step and year
 *
 * $Date: Sunday 26 August  08:19:08 NZST 2018 $
 */

#ifndef AGE_REPORTS_PARTITION_YEARCROSSAGEMATRIX_H_
#define AGE_REPORTS_PARTITION_YEARCROSSAGEMATRIX_H_

// Headers
#include "Reports/Report.h"

// Namespaces
namespace niwa {
namespace reports {
namespace age {

/**
 *
 */
class Partition_YearCrossAgeMatrix : public niwa::Report {
public:
  Partition_YearCrossAgeMatrix(Model* model);
  virtual                     ~Partition_YearCrossAgeMatrix() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final { };
  void                        DoPrepare();
  void                        DoFinalise();
};

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */

#endif /* AGE_REPORTS_PARTITION_YEARCROSSAGEMATRIX_H_ */
