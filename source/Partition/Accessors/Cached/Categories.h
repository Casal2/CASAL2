/**
 * @file Categories.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
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
#include <vector>
#include <string>

#include "Model/Model.h"
#include "Partition/Partition.h"
#include "Utilities/Types.h"

// Namespaces
namespace isam {
namespace partition {
namespace accessors {
namespace cached {

using std::map;
using std::vector;
using std::pair;
using std::string;
using isam::utilities::Double;

/**
 * Class Definition
 */
class Categories {
public:
  // Typedef
  typedef vector<partition::Category> DataType;

  // Methods
  Categories(const vector<string>& category_labels);
  virtual                     ~Categories() = default;
  void                        BuildCache();
  DataType::iterator          Begin();
  DataType::iterator          End();
  unsigned                    Size();

private:
  // Members
  vector<string>              category_labels_;
  ModelPtr                    model_;
  DataType                    data_;
};

// Typedef
typedef boost::shared_ptr<isam::partition::accessors::cached::Categories> CachedCategoriesPtr;

} /* namespace cached */
} /* namespace accessors */
} /* namespace partition */
} /* namespace isam */

#endif /* PARTITION_ACCESSORS_CACHED_CATEGORIES_H_ */
