/**
 * @file CategoriesWithAge.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/01/2013
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
#ifndef PARTITION_ACCESSORS_CATEGORIESWITHAGE_H_
#define PARTITION_ACCESSORS_CATEGORIESWITHAGE_H_

// Headers
#include <map>
#include <vector>
#include <string>

#include "Model/Model.h"
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
 * Class definition
 */
class CategoriesWithAge {
public:
  // Typedefs
  typedef vector<std::pair<string, Double*> > DataType;

  // Methods
  CategoriesWithAge() = delete;
  CategoriesWithAge(Model* model, const vector<string>& categories, const unsigned& age);
  virtual                     ~CategoriesWithAge() = default;
  unsigned                    size();
  DataType::iterator          begin();
  DataType::iterator          end();

private:
  // Members
  Model*                      model_;
  map<unsigned, DataType >    data_; // map<year, vector<pair<category, Double*> > >
};

// Typedef
typedef std::shared_ptr<CategoriesWithAge> CategoriesWithAgePtr;

} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */
#endif /* PARTITION_ACCESSORS_CATEGORIESWITHAGE_H_ */
