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
#include "Partition/Partition.h"

// Namespaces
namespace isam {
namespace accessors {

typedef vector<Double>::iterator CategoryIterator;

/**
 *
 */
class Category {
public:
  Category(string category);
  virtual                     ~Category();
  CategoryIterator            begin();
  CategoryIterator            end();

private:
  // Members
  CategoryIterator            iterator_;
};

}/* namespace accessors */
} /* namespace isam */
#endif /* CATEGORY_H_ */
