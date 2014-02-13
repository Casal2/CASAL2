/**
 * @file PartitionMeanWeight.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 7/01/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will print the mean weights and associated info from the size-weight and age-size
 * objects linked to the partition
 */
#ifndef REPORTS_PARTITIONMEANWEIGHT_H_
#define REPORTS_PARTITIONMEANWEIGHT_H_

// headers
#include "Reports/Report.h"

// namespaces
namespace isam {
namespace reports {

/**
 * class definition
 */
class PartitionMeanWeight : public isam::Report {
public:
  // methods
  PartitionMeanWeight();
  virtual                     ~PartitionMeanWeight() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        Execute() override final;
};

} /* namespace reports */
} /* namespace isam */
#endif /* REPORTS_PARTITIONMEANWEIGHT_H_ */
