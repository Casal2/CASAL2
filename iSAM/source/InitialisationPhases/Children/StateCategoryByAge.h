/**
 * @file StateCategoryByAge.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 16/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This initialisation phase will overwrite the partition with the data provided.
 * This will remove any previous values.
 */
#ifndef SOURCE_INITIALISATIONPHASES_CHILDREN_STATECATEGORYBYAGE_H_
#define SOURCE_INITIALISATIONPHASES_CHILDREN_STATECATEGORYBYAGE_H_

// headers
#include "InitialisationPhases/InitialisationPhase.h"
#include "Partition/Accessors/Categories.h"

// namespaces
namespace niwa {
namespace initialisationphases {

namespace accessor = niwa::partition::accessors;

/**
 * Class definition
 */
class StateCategoryByAge : public niwa::InitialisationPhase {
public:
  // methods
  StateCategoryByAge();
  virtual                     ~StateCategoryByAge() = default;
  virtual void                Execute() override final;

protected:
  // methods
  virtual void                DoValidate() override final;
  virtual void                DoBuild() override final;

private:
  // members
  accessor::Categories        partition_;
  vector<string>              category_labels_;
  unsigned                    min_age_ = 0;
  unsigned                    max_age_ = 0;
  unsigned                    column_count_ = 0;
  parameters::TablePtr        n_table_;
  map<string, vector<Double>> n_;
};

} /* namespace initialisationphases */
} /* namespace niwa */

#endif /* SOURCE_INITIALISATIONPHASES_CHILDREN_STATECATEGORYBYAGE_H_ */
