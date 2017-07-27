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
 *
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef COMBINEDCATEGORIES_H_
#define COMBINEDCATEGORIES_H_

// Headers
#include <map>
#include <vector>
#include <string>

#include "Common/Model/Model.h"
#include "Common/Partition/Partition.h"
#include "Common/Utilities/Types.h"

// namespaces
namespace niwa {
namespace partition {
namespace accessors {
namespace cached {

using std::map;
using std::vector;
using std::pair;
using std::string;
using niwa::utilities::Double;

/**
 *
 */
class CombinedCategories {
public:
  // Typedef
  typedef vector<vector<partition::Category> > DataType;

  // Methods
  CombinedCategories() = delete;
  CombinedCategories(Model* model, const vector<string>& category_labels);
  virtual                     ~CombinedCategories() = default;
  void                        BuildCache();
  DataType::iterator          Begin();
  DataType::iterator          End();
  unsigned                    Size();

private:
  // Members
  vector<vector<string> >     category_labels_;
  Model*                      model_;
  DataType                    data_;
};

// Typedef
typedef std::shared_ptr<niwa::partition::accessors::cached::CombinedCategories> CachedCombinedCategoriesPtr;

} /* namespace cached */
} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */
#endif /* COMBINEDCATEGORIES_H_ */
