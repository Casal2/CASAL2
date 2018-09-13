/**
 * @file PartitionBiomass.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 18/12/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGE_REPORTS_PARTITIONBIOMASS_H_
#define AGE_REPORTS_PARTITIONBIOMASS_H_

// headers
#include "Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {
namespace age {

/**
 * Class definition
 */
class PartitionBiomass : public Report {
public:
  PartitionBiomass(Model* model);
  virtual                     ~PartitionBiomass() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final { };

private:
  string                      units_;
};

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */

#endif /* AGE_REPORTS_PARTITIONBIOMASS_H_ */
