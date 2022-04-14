/**
 * @file PartitionBiomass.h
 * @author  C.Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will print the partition for the target
 * time step and year
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef REPORTS_PARTITION_BIOMASS_H_
#define REPORTS_PARTITION_BIOMASS_H_

// Headers
#include "../../Reports/Report.h"

// Namespaces
namespace niwa {
namespace reports {

/**
 *
 */
class PartitionBiomass : public niwa::Report {
public:
  PartitionBiomass();
  virtual ~PartitionBiomass() = default;
  void DoValidate(shared_ptr<Model> model) override final;
  void DoBuild(shared_ptr<Model> model) override final{};
  void DoExecute(shared_ptr<Model> model) override final;
  void DoPrepareTabular(shared_ptr<Model> model) override final;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* AGE_REPORTS_PARTITION_H_ */
