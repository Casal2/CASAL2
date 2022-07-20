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
#include <string>
#include <vector>

#include "../../Model/Model.h"
#include "../../Partition/Partition.h"
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
 * Class Definitions
 */
class CombinedCategories {
public:
  // Typedef
  typedef vector<vector<partition::Category*> > DataType;  
  /* the first element is a 'group' of combined categories the second is specific combinations for example
  male+female  : one group with two combinations or DataType.size() == 1 and DataType[0].size == 2
  male.east+male.west  female.east+female.west  : two groups with each having two combinations or DataType.size() == 2 and DataType[0].size == 2
  */
  // Methods
  CombinedCategories() = delete;
  CombinedCategories(shared_ptr<Model> model, const vector<string>& category_labels);
  virtual ~CombinedCategories() = default;
  DataType::iterator Begin();
  DataType::iterator End();
  partition::Category* GetCategoryFromCombinedCategoryByIndex(unsigned group_index, unsigned combined_index);
  unsigned           Size();
  unsigned           category_count() const { return category_count_; }
  // Not a super efficient function below, should only be called in Validate or Build.
  vector<unsigned>   get_category_index(string category_label); // returns a vector first element is the group_index_, second element is the combined_index_

private:
  // Members
  shared_ptr<Model>       model_;
  map<unsigned, DataType> data_;
  unsigned                category_count_;
  vector<string>          category_labels_;
  vector<unsigned>        group_index_;
  vector<unsigned>        combined_index_;

};

// Typedef
typedef std::shared_ptr<CombinedCategories> CombinedCategoriesPtr;

} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */
#endif /* COMBINEDCATEGORIES_H_ */
