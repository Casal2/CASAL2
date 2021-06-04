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
#include <string>
#include <vector>

#include "../../../Model/Model.h"
#include "../../../Partition/Partition.h"
#include "../../../Utilities/Types.h"

// namespaces
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
 *
 */
class CombinedCategories {
public:
  // Typedef
  typedef vector<vector<partition::Category> > DataType;

  // Methods
  CombinedCategories() = delete;
  CombinedCategories(shared_ptr<Model> model, const vector<string>& category_labels);
  virtual ~CombinedCategories() = default;
  void               BuildCache();
  DataType::iterator Begin();
  DataType::iterator End();
  unsigned           Size();

  bool needs_rebuild() const { return need_rebuild_; }

private:
  // Members
  vector<vector<string> > category_labels_;
  shared_ptr<Model>       model_;
  DataType                data_;
  bool                    need_rebuild_ = true;
};

// Typedef
typedef std::shared_ptr<niwa::partition::accessors::cached::CombinedCategories> CachedCombinedCategoriesPtr;

} /* namespace cached */
} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */
#endif /* COMBINEDCATEGORIES_H_ */
