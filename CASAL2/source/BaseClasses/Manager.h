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
#ifndef OLDBASE_MANAGER_H_
#define OLDBASE_MANAGER_H_

// Headers
#include <string>
#include <string_view>
#include <vector>

#include "../Utilities/NoCopy.h"
#include "../Model/Managers.h"

// Namespaces
namespace niwa {
class Model;
using std::string_view;

namespace base {
using std::vector;
using std::string;

// classes
template <class ClassType, class StoredType>
class Manager {
public:
  // Methods
  virtual                     ~Manager() {
    for (StoredType* object: objects_)
      delete object;
  }
  virtual void                Validate();
  virtual void                Build();
  virtual void                Reset();
  void                        AddObject(StoredType* object) { objects_.push_back(object); }
  virtual void                Clear() { objects_.clear(); }
  bool                        HasType(std::string_view type);

  // Accessors/mutators
  StoredType*									get(string_view label);
  vector<StoredType*>         objects() { return objects_; }
  unsigned                    size() { return objects_.size(); }

protected:
  // Methods
  Manager() = default;

  // Members
  vector<StoredType*> objects_;

  // Macros
  DISALLOW_COPY_MOVE_AND_ASSIGN(Manager);
};

} /* namespace base */
} /* namespace niwa */

#include "Manager-inl.h"

#endif /* BASE_MANAGER_H_ */
