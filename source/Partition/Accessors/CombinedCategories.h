/**
 * @file CombinedCategories.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This accessor works the same way as the Categories
 * accessor except it supports combination categories (e.g male+female male unsexed)
 * and
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PARTITION_ACCESSORS_COMBINEDCATEGORIES_H_
#define PARTITION_ACCESSORS_COMBINEDCATEGORIES_H_

// Headers
#include <map>
#include <vector>
#include <string>

#include "Model/Model.h"
#include "Partition/Partition.h"
#include "Utilities/Types.h"

// Namespaces
namespace isam {
namespace partition {
namespace accessors {

using std::map;
using std::vector;
using std::pair;
using std::string;
using isam::utilities::Double;

/**
 * Class Definitions
 */
class CombinedCategories {
public:
  // Typedef
  typedef vector<vector<partition::Category*> > DataType;

  // Methods
  CombinedCategories(const vector<string>& category_labels);
  virtual                     ~CombinedCategories() = default;
  DataType::iterator          Begin();
  DataType::iterator          End();
  unsigned                    Size();

private:
  // Members
  ModelPtr                    model_;
  map<unsigned, DataType>     data_;
};

// Typedef
typedef boost::shared_ptr<CombinedCategories> CombinedCategoriesPtr;

} /* namespace accessors */
} /* namespace partition */
} /* namespace isam */
#endif /* COMBINEDCATEGORIES_H_ */
