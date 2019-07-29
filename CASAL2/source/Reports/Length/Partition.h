/**
 * @file Partition.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will print the partition for the target
 * time step and year
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef LENGTH_REPORTS_PARTITION_H_
#define LENGTH_REPORTS_PARTITION_H_

// Headers
#include "Reports/Report.h"

// Namespaces
namespace niwa {
namespace reports {
namespace length {


/**
 *
 */
class Partition : public niwa::Report {
public:
  Partition(Model* model);
  virtual                     ~Partition() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final { };
};

} /* namespace length */
} /* namespace reports */
} /* namespace niwa */
#endif /* LENGTH_REPORTS_PARTITION_H_ */
