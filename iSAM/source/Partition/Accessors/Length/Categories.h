/**
 * @file Categories.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 18/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This accessor will form a link to the partition that is filtered
 * by one or more categories.
 */
#ifndef PARTITION_ACCESSORS_LENGTH_CATEGORIES_H_
#define PARTITION_ACCESSORS_LENGTH_CATEGORIES_H_

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
namespace length {

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
  typedef vector< map<unsigned, map<unsigned, Double>>* > DataType;

  // Methods
  Categories();
  virtual                     ~Categories() = default;
  void                        Init(const vector<string>& category_labels);
  DataType::iterator          begin();
  DataType::iterator          end();
  unsigned                    size();

private:
  // Members
  ModelPtr                    model_;
  map<unsigned, DataType>     data_; // map<year, DataType>
};

// Typedef
typedef boost::shared_ptr<niwa::partition::accessors::length::Categories> LengthCategoriesPtr;

} /* namespace length */
} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */
#endif /* PARTITION_ACCESSORS_CATEGORIES_H_ */
