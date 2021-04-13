/**
 * @file PartitionMeanWeight.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 7/01/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will print the mean weights and associated info from the size-weight and age-size
 * objects linked to the partition
 */
#ifndef AGE_REPORTS_PARTITIONMEANWEIGHT_H_
#define AGE_REPORTS_PARTITIONMEANWEIGHT_H_

// headers
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {
namespace age {

/**
 * class definition
 */
class PartitionMeanWeight : public niwa::Report {
public:
  // methods
  PartitionMeanWeight();
  virtual                     ~PartitionMeanWeight() = default;
  void                        DoValidate(shared_ptr<Model> model) final { };
  void                        DoBuild(shared_ptr<Model> model) final;
  void                        DoExecute(shared_ptr<Model> model) final;
  void                        DoExecuteTabular(shared_ptr<Model> model) final { };
};

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */
#endif /* AGE_REPORTS_PARTITIONMEANWEIGHT_H_ */
