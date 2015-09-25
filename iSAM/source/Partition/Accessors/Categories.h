/**
 * @file Categories.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
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
#include <vector>
#include <string>

#include "Model/Model.h"
#include "Partition/Partition.h"
#include "Utilities/Types.h"

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
 * Class Definition
 */
class Categories {
public:
  // Typedef
  typedef vector<partition::Category*> DataType;

  // Methods
  Categories();
  virtual                     ~Categories() = default;
  void                        Init(const vector<string>& category_labels);
  DataType::iterator          begin();
  DataType::iterator          end();
  unsigned                    size();

private:
  // Members
  Model*                    model_;
  map<unsigned, DataType>     data_;
};

// Typedef
typedef std::shared_ptr<niwa::partition::accessors::Categories> CategoriesPtr;

} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */
#endif /* PARTITION_ACCESSORS_CATEGORIES_H_ */
