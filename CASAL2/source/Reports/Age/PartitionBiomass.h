/**
 * @file PartitionBiomass.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 18/12/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGE_REPORTS_PARTITIONBIOMASS_H_
#define AGE_REPORTS_PARTITIONBIOMASS_H_

// headers
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {
namespace age {

/**
 * Class definition
 */
class PartitionBiomass : public Report {
public:
  PartitionBiomass();
  virtual ~PartitionBiomass() = default;
  void DoValidate(shared_ptr<Model> model) final;
  void DoBuild(shared_ptr<Model> model) final{};
  void DoExecute(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final;

private:
  string units_;
};

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */

#endif /* AGE_REPORTS_PARTITIONBIOMASS_H_ */
