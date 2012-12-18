/**
 * @file Categories.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class represents the category configuration for the partition
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef CATEGORIES_H_
#define CATEGORIES_H_

// Headers
#include <boost/shared_ptr.hpp>

#include "BaseClasses/Object.h"

// Namespaces
namespace isam {

using boost::shared_ptr;

/**
 * Struct Definition
 */
struct CategoryInfo {
  string name_ = "";
  vector<unsigned>  years_;
  unsigned          min_age_ = 0;
  unsigned          max_age_ = 0;
};

/**
 * Class Definition
 */
class Categories : public isam::base::Object {
public:
  // Methods
  static shared_ptr<Categories> Instance();
  virtual                       ~Categories() = default;
  void                          Validate();

  // Accessors
  string                        format() { return format_; }
  vector<string>                category_names() { return category_names_; }

private:
  // Methods
  Categories();

  // Members
  string                      format_;
  vector<string>              names_;
  vector<string>              category_names_;
  map<string, CategoryInfo>   categories_;

};

// Typedef
typedef boost::shared_ptr<Categories> CategoriesPtr;

} /* namespace isam */

#endif /* CATEGORIES_H_ */
