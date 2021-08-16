/**
 * @file Objects.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 1/09/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class provides accessor methods to find objects
 * with the model. It's attached to the model class so we
 * don't have to worry about singletons etc.
 */
#ifndef SOURCE_MODEL_OBJECTS_H_
#define SOURCE_MODEL_OBJECTS_H_

#include <memory>
#include <string>

#include "../BaseClasses/Object.h"
#include "../Translations/Translations.h"
#include "../Utilities/Map.h"
#include "../Utilities/NoCopy.h"
#include "../Utilities/Types.h"

// namespaces
namespace niwa {

class Model;
class TimeVarying;
namespace estimates {
class Creator;
}
using niwa::utilities::Double;
using std::string;
using utilities::OrderedMap;

// classes
class Objects {
  friend class MockObjects;
  friend class Model;
  friend class estimates::Creator;
  friend class TimeVarying;

public:
  // methods
  virtual ~Objects() = default;
  virtual bool                        VerifyAddressableForUse(const string& parameter_absolute_name, addressable::Usage usage, string& error);
  virtual addressable::Type           GetAddressableType(const string& parameter_absolute_name);
  virtual Double*                     GetAddressable(const string& addressable_absolute_name);
  virtual vector<Double*>*            GetAddressables(const string& addressable_absolute_name);
  virtual map<unsigned, Double>*      GetAddressableUMap(const string& addressable_absolute_name);
  virtual OrderedMap<string, Double>* GetAddressableSMap(const string& addressable_absolute_name);
  virtual vector<Double>*             GetAddressableVector(const string& addressable_absolute_name);
  virtual base::Object*               FindObject(const string& parameter_absolute_name);

protected:
  // methods
  Objects() = delete;
  explicit Objects(shared_ptr<Model> model);
  virtual base::Object*             FindObjectOrNull(const string& parameter_absolute_name);
  virtual void                      ExplodeString(const string& parameter_absolute_name, string& type, string& label, string& addressable, string& index);
  virtual void                      ImplodeString(const string& type, const string& label, const string& parameter, const string& index, string& target_parameter);
  virtual std::pair<string, string> ExplodeParameterAndIndex(const string& parameter_absolute_name);

  // members
  shared_ptr<Model> model_ = nullptr;
};

} /* namespace niwa */

#endif /* SOURCE_MODEL_OBJECTS_H_ */
