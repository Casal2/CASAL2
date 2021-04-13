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
#include "../../Reports/Report.h"

// Namespaces
namespace niwa {
namespace reports {
namespace age {

/**
 *
 */
class Partition_YearCrossAgeMatrix : public niwa::Report {
public:
  Partition_YearCrossAgeMatrix();
  virtual                     ~Partition_YearCrossAgeMatrix() = default;
  void                        DoValidate(shared_ptr<Model> model) final;
  void                        DoBuild(shared_ptr<Model> model) final { };
  void                        DoExecute(shared_ptr<Model> model) final;
  void                        DoExecuteTabular(shared_ptr<Model> model) final { };
  void                        DoPrepare(shared_ptr<Model> model) final;
  void                        DoFinalise(shared_ptr<Model> model) final;
};

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */

#endif /* AGE_REPORTS_PARTITION_YEARCROSSAGEMATRIX_H_ */
