/**
 * @file CombinedCategories.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
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

#include "Common/Model/Model.h"
#include "Common/Partition/Partition.h"
#include "Common/Utilities/Types.h"

// Namespaces
namespace niwa {
namespace partition {
namespace accessors {

using std::map;
using std::vector;
using std::pair;
using std::string;
using niwa::utilities::Double;

/**
 * Class Definitions
 */
class CombinedCategories {
public:
  // Typedef
  typedef vector<vector<partition::Category*> > DataType;

  // Methods
  CombinedCategories() = delete;
  CombinedCategories(Model* model, const vector<string>& category_labels);
  virtual                     ~CombinedCategories() = default;
  DataType::iterator          Begin();
  DataType::iterator          End();
  unsigned                    Size();
  unsigned                    category_count() const { return category_count_; }

private:
  // Members
  Model*                    model_;
  map<unsigned, DataType>     data_;
  unsigned                    category_count_;
};

// Typedef
typedef std::shared_ptr<CombinedCategories> CombinedCategoriesPtr;

} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */
#endif /* COMBINEDCATEGORIES_H_ */
