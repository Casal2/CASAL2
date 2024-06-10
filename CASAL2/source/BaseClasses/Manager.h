/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/10/2012
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
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
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "../Model/Managers.h"
#include "../Utilities/NoCopy.h"


// Namespaces
namespace niwa {
class Model;
using std::string_view;

namespace base {
using std::string;
using std::vector;

// classes
template <class ClassType, class StoredType>
class Manager {
public:
  // Methods
  virtual ~Manager();
  virtual void Validate();
  virtual void Build();
  virtual void Verify(shared_ptr<Model> model);
  virtual void Reset();
  virtual void AddObject(StoredType* object) { objects_.push_back(object); }
  virtual void Clear() { objects_.clear(); }
  bool         HasType(std::string_view type);

  // Accessors/mutators
  StoredType*         get(string_view label);
  vector<StoredType*> objects() { return objects_; }
  unsigned            size() { return objects_.size(); }

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
