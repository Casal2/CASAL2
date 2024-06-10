/**
 * @file TransitionCategory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 21/12/2012
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This class is a category transition/shift class that allows you to
 * mature fish from 1 category to another. This is used when maturing
 * fish from an immature to mature stage etc.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef TRANSITIONCATEGORY_H_
#define TRANSITIONCATEGORY_H_

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
 * Class definition
 */
class TransitionCategory : public niwa::Process {
public:
  // Methods
  explicit TransitionCategory(shared_ptr<Model> model);
  virtual ~TransitionCategory() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final;

private:
  // Members
  vector<string>                            from_category_names_;
  vector<string>                            to_category_names_;
  vector<Double>                            proportions_;
  OrderedMap<string, Double>                proportions_by_category_;
  vector<string>                            selectivity_names_;
  accessor::Categories                      from_partition_;
  accessor::Categories                      to_partition_;
  vector<Selectivity*>                      selectivities_;
  vector<vector<Double>>                    abundance_to_move_categories_;
  unsigned                                  min_age_                       = model_->min_age();
  bool                                      process_is_in_mortality_block_ = false;
  map<unsigned, OrderedMap<string, Double>> proportions_by_year_;  // n_years x n_categories
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* TRANSITIONCATEGORY_H_ */
