/**
 * @file Category.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 29/10/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is a category accessor for the world partition.
 * It'll allow the iteration over a specific category in our world.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PARTITION_ACCESSORS_CATEGORY_H_
#define PARTITION_ACCESSORS_CATEGORY_H_

// Headers
#include <map>
#include <string>
#include <vector>

#include "../../Model/Model.h"
#include "../../Utilities/Types.h"

// Namespaces
namespace niwa {
namespace partition {
namespace accessors {

using niwa::utilities::Double;
using std::map;
using std::pair;
using std::string;
using std::vector;

/**
 *
 */
class Category {
public:
  // Typedefs
  typedef vector<Double>* DataType;

  // Methods
  Category() = delete;
  Category(shared_ptr<Model> model, const string& category_name);
  virtual ~Category() = default;
  //  DataType::iterator          begin();
  //  DataType::iterator          end();

private:
  // Members
  shared_ptr<Model>       model_;
  map<unsigned, DataType> data_;
};

} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */
#endif /* PARTITION_ACCESSORS_CATEGORY_H_ */
