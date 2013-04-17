/**
 * @file MaturationRate.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 21/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is a category transition/shift class that allows you to
 * mature fish from 1 category to another. This is used when maturing
 * fish from an immature to mature stage etc.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef MATURATIONRATE_H_
#define MATURATIONRATE_H_

// Headers
#include "Processes/Process.h"

#include "Partition/Accessors/Categories.h"
#include "Selectivities/Selectivity.h"

// Namespaces
namespace isam {
namespace processes {

namespace accessor = isam::partition::accessors;

/**
 * Class defintiion
 */
class MaturationRate : public isam::Process {
public:
  // Methods
  MaturationRate();
  virtual                     ~MaturationRate() = default;
  void                        Validate() override final;
  void                        Build() override final;
  void                        Execute() override final;
  void                        Reset() override final;

private:
  // Members
  vector<string>              from_category_names_;
  vector<string>              to_category_names_;
  vector<Double>              proportions_;
  vector<string>              selectivity_names_;
  accessor::CategoriesPtr     from_partition_;
  accessor::CategoriesPtr     to_partition_;
  vector<SelectivityPtr>      selectivities_;
  vector<vector<Double> >     maturation_rates_;

};

} /* namespace processes */
} /* namespace isam */
#endif /* MATURATIONRATE_H_ */
