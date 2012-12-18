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
#ifndef CATEGORY_H_
#define CATEGORY_H_

// Headers
#include <map>
#include <vector>
#include <string>

#include "Utilities/Types.h"

// Namespaces
namespace isam {
namespace partition {
namespace accessors {

using std::map;
using std::vector;
using std::pair;
using std::string;
using isam::utilities::Double;

/**
 *
 */
class Category {
private:
  // Typedefs
  typedef map<unsigned, vector<pair<string, vector<Double>& > > > CategoryMap; // map<year, vector<pair<category, vector<values>>>>
  typedef vector<pair<string, vector<Double>& > >  CategoryInfo; // vector<pair<category, values>>

public:
  // Methods
  Category();
  virtual                     ~Category();
  void                        Begin();
  bool                        Next();

  // Accessors
  vector<Double>&             current_values();
  string                      current_category();

private:
  // Members
  CategoryMap                 category_map_;
  CategoryInfo::iterator      category_iter_;
};

}/* namespace accessors */
}/* namespace partition */
} /* namespace isam */
#endif /* CATEGORY_H_ */
