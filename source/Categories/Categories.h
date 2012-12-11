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
 * Class Definition
 */
class Categories : public isam::base::Object {
public:
  // Methods
  static shared_ptr<Categories> Instance();
  virtual                       ~Categories() = default;

private:
  // Methods
  Categories();

};

// Typedef
typedef boost::shared_ptr<Categories> CategoriesPtr;

} /* namespace isam */

#endif /* CATEGORIES_H_ */
