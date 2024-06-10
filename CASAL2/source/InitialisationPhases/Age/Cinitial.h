/**
 * @file Cinitial.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 16/11/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This initialisation phase will overwrite the partition with the data provided.
 * This will remove any previous values.
 */
#ifndef SOURCE_INITIALISATIONPHASES_CHILDREN_CINITIAL_H_
#define SOURCE_INITIALISATIONPHASES_CHILDREN_CINITIAL_H_

// headers
#include "../../InitialisationPhases/InitialisationPhase.h"
#include "../../Partition/Accessors/Cached/CombinedCategories.h"
#include "../../Partition/Accessors/CombinedCategories.h"

// namespaces
namespace niwa {
class TimeStep;
class DerivedQuantity;
namespace initialisationphases {
namespace age {
using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;

/**
 * Class definition
 */
class Cinitial : public niwa::InitialisationPhase {
public:
  // methods
  explicit Cinitial(shared_ptr<Model> model);
  virtual ~Cinitial();

protected:
  // methods
  void DoValidate() override final;
  void DoBuild() override final;
  void DoExecute() override final;
private:
  // members
  CombinedCategoriesPtr       partition_;
  CachedCombinedCategoriesPtr cached_partition_;
  vector<string>              category_labels_;
  unsigned                    min_age_      = 0;
  unsigned                    max_age_      = 0;
  unsigned                    column_count_ = 0;
  parameters::Table*          n_table_      = nullptr;
  map<string, vector<Double>> n_;
  vector<Double>              binitial_;
  vector<Double>              rinitial_;
  vector<TimeStep*>           time_steps_;
  vector<string>              derived_quanitity_;
  vector<DerivedQuantity*>    derived_ptr_;
  unsigned                    ssb_offset_ = 1;
};

} /* namespace age */
} /* namespace initialisationphases */
} /* namespace niwa */

#endif /* SOURCE_INITIALISATIONPHASES_CHILDREN_CINITIAL_H_ */
