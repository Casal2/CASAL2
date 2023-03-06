/**
 * @file MarkovianMovement.h
 * @author  Craig Marsh
 * @version 1.0
 * @date 2023
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is a category transition process that will move fish from multiple categories to multiple other categories
 * In an attempt to allow Markovian movement
 *
 */
#ifndef MARKOVIANMOVEMENT_H_
#define MARKOVIANMOVEMENT_H_

// Headers
#include "Partition/Accessors/Categories.h"
#include "Processes/Process.h"
#include "Selectivities/Selectivity.h"

// Namespaces
namespace niwa {
namespace processes {
namespace age {

namespace accessor = niwa::partition::accessors;
using utilities::OrderedMap;

/**
 * Class defintiion
 */
class MarkovianMovement : public niwa::Process {
public:
  // Methods
  explicit MarkovianMovement(shared_ptr<Model> model);
  virtual ~MarkovianMovement() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoExecute() override final;

private:
  // Members
  vector<string>             from_category_names_;
  vector<string>             to_category_names_;
  vector<Double>             proportions_;
  OrderedMap<string, Double> proportions_by_category_;
  vector<string>             selectivity_names_;
  accessor::Categories       from_partition_;
  accessor::Categories       to_partition_;
  vector<Selectivity*>       selectivities_;
  vector<vector<Double>>     abundance_to_move_categories_;
  vector<string>             category_lookup_table_;

  unsigned                   min_age_ = model_->min_age();

  map<string, map<string, Double>> from_to_proportions_;
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* MARKOVIANMOVEMENT_H_ */
