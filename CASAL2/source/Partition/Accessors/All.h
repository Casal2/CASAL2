/**
 * @file All.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 10/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This accessors will give a complete view of the partition.
 *
 * NOTE: This accessor ignores any specific rules a category
 * may have regarding visibility during specific years. It will
 * always print every category in the model regardless of year.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PARTITION_ACCESSORS_ALL_H_
#define PARTITION_ACCESSORS_ALL_H_

// Headers
#include <map>
#include <vector>
#include <string>

#include "../../Partition/Partition.h"
#include "../../Utilities/Types.h"

// Namespaces
namespace niwa {
class Model;

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
class All {
public:
  // Typedefs
  typedef vector<partition::Category*> DataType;

  // Methods
  All() = delete;
  explicit All(shared_ptr<Model> model);
  virtual                     ~All() = default;
  DataType::iterator          begin();
  DataType::iterator          end();
  DataType::iterator          Begin(); // TODO: Remove these
  DataType::iterator          End();
  unsigned                    Size();

private:
  // Members
  shared_ptr<Model>           model_;
  All::DataType               data_;
};

typedef std::shared_ptr<niwa::partition::accessors::All> AllPtr;

} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */
#endif /* PARTITION_ACCESSORS_ALL_H_ */
