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
#include "Common/InitialisationPhases/InitialisationPhase.h"
#include "Common/Partition/Accessors/Categories.h"

// namespaces
namespace niwa {
namespace age {
namespace initialisationphases {
namespace accessor = niwa::partition::accessors;

/**
 * Class definition
 */
class StateCategoryByAge : public niwa::InitialisationPhase {
public:
  // methods
  explicit StateCategoryByAge(Model* model);
  virtual                     ~StateCategoryByAge();
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
  parameters::Table*          n_table_ = nullptr;
  map<string, vector<Double>> n_;
};

} /* namespace initialisationphases */
} /* namespace age */
} /* namespace niwa */

#endif /* SOURCE_INITIALISATIONPHASES_CHILDREN_STATECATEGORYBYAGE_H_ */
