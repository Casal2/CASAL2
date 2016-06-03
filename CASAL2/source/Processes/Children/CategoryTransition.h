/**
 * @file CategoryTransition.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 21/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is a category transition/shift class that allows you to
 * mature fish from 1 category to another. This is used when maturing
 * fish from an immature to mature stage etc.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef CATEGORY_TRANSITION_H_
#define CATEGORY_TRANSITION_H_

// Headers
#include "Processes/Process.h"
#include "Partition/Accessors/Categories.h"
#include "Selectivities/Selectivity.h"

// Namespaces
namespace niwa {
namespace processes {

namespace accessor = niwa::partition::accessors;
using utilities::OrderedMap;

/**
 * Class defintiion
 */
class CategoryTransition : public niwa::Process {
public:
  // Methods
  explicit CategoryTransition(Model* model);
  virtual                     ~CategoryTransition() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;
  void                        DoExecute() override final;

private:
  // Members
  vector<string>              from_category_names_;
  vector<string>              to_category_names_;
  vector<Double>              proportions_;
  OrderedMap<string, Double>  proportions_by_category_;
  vector<string>              selectivity_names_;
  accessor::Categories        from_partition_;
  accessor::Categories        to_partition_;
  vector<Selectivity*>        selectivities_;
  vector<vector<Double> >     transition_rates_;

};

} /* namespace processes */
} /* namespace niwa */
#endif /* CATEGORY_TRANSITION_H_ */
