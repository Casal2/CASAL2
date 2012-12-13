/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/10/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is our base manager class. It's a template base class
 * for storing containers of objects and running generic state-change
 * methods *e.g validate/verify etc* on them as well as providing accessors.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef BASE_MANAGER_H_
#define BASE_MANAGER_H_

// Headers
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "Utilities/NoCopy.h"

// Namespaces
namespace isam {
namespace base {

using boost::shared_ptr;
using std::vector;

/**
 * Class definition
 */
template <class ClassType, class StoredType>
class Manager {
public:
  // Methods
  virtual                     ~Manager() = default;
  static ClassType&           Instance();
  void                        AddObject(shared_ptr<StoredType> object) {
    objects_.push_back(object);
  }

protected:
  // Methods
  Manager() = default;

  // Members
  vector<shared_ptr<StoredType> >     objects_;

  // Macros
  DISALLOW_COPY_AND_ASSIGN(Manager);
};

} /* namespace base */
} /* namespace isam */

#include "Manager-inl.h"

#endif /* BASE_MANAGER_H_ */
