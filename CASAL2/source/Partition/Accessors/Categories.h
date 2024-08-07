/**
 * @file Categories.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/02/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This accessor will form a link to the partition that is filtered
 * by one or more categories.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PARTITION_ACCESSORS_CATEGORIES_H_
#define PARTITION_ACCESSORS_CATEGORIES_H_

// Headers
#include <map>
#include <string>
#include <vector>

#include "../../Model/Model.h"
#include "../../Partition/Partition.h"
#include "../../Utilities/Types.h"

// Namespaces
namespace niwa {
namespace partition {
namespace accessors {

using ::niwa::utilities::Double;
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
  typedef vector<partition::Category*> DataType;

  // Methods
  Categories() = delete;
  explicit Categories(shared_ptr<Model> model);
  virtual ~Categories() = default;
  void                     Init(const vector<string>& category_labels);
  DataType::const_iterator begin();
  DataType::const_iterator end();
  unsigned                 size();

private:
  // Members
  shared_ptr<Model>       model_;
  map<unsigned, DataType> data_;
  unsigned                year_offset_ = 0;  // used for lookup on vector
};

// Typedef
typedef std::shared_ptr<niwa::partition::accessors::Categories> CategoriesPtr;

} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */
#endif /* PARTITION_ACCESSORS_CATEGORIES_H_ */
