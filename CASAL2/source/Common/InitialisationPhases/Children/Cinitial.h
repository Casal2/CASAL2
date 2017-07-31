/**
 * @file Cinitial.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 16/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This initialisation phase will overwrite the partition with the data provided.
 * This will remove any previous values.
 */
#ifndef SOURCE_INITIALISATIONPHASES_CHILDREN_CINITIAL_H_
#define SOURCE_INITIALISATIONPHASES_CHILDREN_CINITIAL_H_

// headers
#include "Common/InitialisationPhases/InitialisationPhase.h"
#include "Common/Partition/Accessors/CombinedCategories.h"
#include "Common/Partition/Accessors/Cached/CombinedCategories.h"
#include "Age/Processes/Children/RecruitmentBevertonHolt.h"
// TODO: move this to age

// namespaces
namespace niwa {
class TimeStep;
class DerivedQuantity;

namespace initialisationphases {
using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;
using age::processes::RecruitmentBevertonHolt;

/**
 * Class definition
 */
class Cinitial : public niwa::InitialisationPhase {
public:
  // methods
  explicit Cinitial(Model* model);
  virtual                     ~Cinitial();
  virtual void                Execute() override final;

protected:
  // methods
  virtual void                DoValidate() override final;
  virtual void                DoBuild() override final;

private:
  // members
  CombinedCategoriesPtr       partition_;
  CachedCombinedCategoriesPtr cached_partition_;
  vector<string>              category_labels_;
  unsigned                    min_age_ = 0;
  unsigned                    max_age_ = 0;
  unsigned                    column_count_ = 0;
  parameters::Table*          n_table_ = nullptr;
  map<string, vector<Double>> n_;
  vector<Double>              binitial_;
  vector<Double>              rinitial_;
  vector<TimeStep*>           time_steps_;
  vector<string>              derived_quanitity_;
  vector<DerivedQuantity*>    derived_ptr_;
  unsigned                    ssb_offset_ = 1;
};

} /* namespace initialisationphases */
} /* namespace niwa */

#endif /* SOURCE_INITIALISATIONPHASES_CHILDREN_CINITIAL_H_ */
