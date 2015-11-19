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
#include "InitialisationPhases/InitialisationPhase.h"
#include "Partition/Accessors/CombinedCategories.h"

// namespaces
namespace niwa {
namespace initialisationphases {

using partition::accessors::CombinedCategoriesPtr;

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
  vector<string>              category_labels_;
  unsigned                    min_age_ = 0;
  unsigned                    max_age_ = 0;
  unsigned                    column_count_ = 0;
  parameters::Table*          n_table_ = nullptr;
  map<string, vector<Double>> n_;
};

} /* namespace initialisationphases */
} /* namespace niwa */

#endif /* SOURCE_INITIALISATIONPHASES_CHILDREN_CINITIAL_H_ */
