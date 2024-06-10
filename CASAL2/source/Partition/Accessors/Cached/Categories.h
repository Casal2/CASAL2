/**
 * @file Categories.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/03/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/�2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PARTITION_ACCESSORS_CACHED_CATEGORIES_H_
#define PARTITION_ACCESSORS_CACHED_CATEGORIES_H_

// Headers
#include <map>
#include <string>
#include <vector>

#include "../../../Model/Model.h"
#include "../../../Partition/Partition.h"
#include "../../../Utilities/Types.h"

// Namespaces
namespace niwa {
namespace partition {
namespace accessors {
namespace cached {

using niwa::utilities::Double;
using std::map;
using std::pair;
using std::string;
using std::vector;

/**
 * Class Definition
 */
class Categories {
public:
  // Typedef
  typedef vector<partition::Category> DataType;

  // Methods
  Categories() = default;
  explicit Categories(shared_ptr<Model> model);
  virtual ~Categories() = default;
  void               Init(const vector<string>& category_labels);
  void               BuildCache();
  DataType::iterator begin();
  DataType::iterator end();
  unsigned           size();

private:
  // Members
  shared_ptr<Model> model_;
  vector<string>    category_labels_;
  DataType          data_;
};

// Typedef
typedef std::shared_ptr<niwa::partition::accessors::cached::Categories> CachedCategoriesPtr;

} /* namespace cached */
} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */

#endif /* PARTITION_ACCESSORS_CACHED_CATEGORIES_H_ */
