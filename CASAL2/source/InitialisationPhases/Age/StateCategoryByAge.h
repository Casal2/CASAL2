/**
 * @file StateCategoryByAge.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 16/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This initialisation phase will overwrite the partition with the data provided.
 * This will remove any previous values.
 */
#ifndef SOURCE_INITIALISATIONPHASES_CHILDREN_STATECATEGORYBYAGE_H_
#define SOURCE_INITIALISATIONPHASES_CHILDREN_STATECATEGORYBYAGE_H_

// headers
#include "../../InitialisationPhases/InitialisationPhase.h"
#include "../../Partition/Accessors/Cached/Categories.h"
#include "../../Partition/Accessors/Categories.h"

// namespaces
namespace niwa {
namespace initialisationphases {
namespace age {
namespace accessor = niwa::partition::accessors;

/**
 * Class definition
 */
class StateCategoryByAge : public niwa::InitialisationPhase {
public:
  // methods
  explicit StateCategoryByAge(shared_ptr<Model> model);
  virtual ~StateCategoryByAge();

protected:
  // methods
  void DoValidate() override final;
  void DoBuild() override final;
  void DoExecute() override final;

private:
  // members
  accessor::Categories         partition_;
  accessor::cached::Categories cached_partition_;
  vector<string>               category_labels_;
  unsigned                     min_age_      = 0;
  unsigned                     max_age_      = 0;
  unsigned                     column_count_ = 0;
  parameters::Table*           n_table_      = nullptr;
  map<string, vector<Double>>  n_;
};

} /* namespace age */
} /* namespace initialisationphases */
} /* namespace niwa */

#endif /* SOURCE_INITIALISATIONPHASES_CHILDREN_STATECATEGORYBYAGE_H_ */
