/**
 * @file Partition.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will print the partition for the target
 * time step and year
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef AGE_REPORTS_PARTITION_H_
#define AGE_REPORTS_PARTITION_H_

// Headers
#include "../../Reports/Report.h"

// Namespaces
namespace niwa {
namespace reports {
namespace age {

/**
 *
 */
class Partition : public niwa::Report {
public:
  Partition();
  virtual                     ~Partition() = default;
  void                        DoValidate(shared_ptr<Model> model) override final;
  void                        DoBuild(shared_ptr<Model> model) override final { };
  void                        DoExecute(shared_ptr<Model> model) override final;
  void                        DoExecuteTabular(shared_ptr<Model> model) override final { };
};

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */

#endif /* AGE_REPORTS_PARTITION_H_ */
