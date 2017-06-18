/**
 * @file Objects.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 1/09/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class provides accessor methods to find objects
 * with the model. It's attached to the model class so we
 * don't have to worry about singletons etc.
 */
#ifndef SOURCE_MODEL_OBJECTS_H_
#define SOURCE_MODEL_OBJECTS_H_

#include <string>
#include <memory>

#include "BaseClasses/Object.h"
#include "Translations/Translations.h"
#include "Utilities/Map.h"
#include "Utilities/NoCopy.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa {

class Model;
namespace estimates { class Creator; }
using niwa::utilities::Double;
using std::string;
using utilities::OrderedMap;

// classes
class Objects {
  friend class Model;
  friend class estimates::Creator;
public:
  // methods
  bool                        VerfiyAddressableForUse(const string& parameter_absolute_name, addressable::Usage usage, string& error);
  addressable::Type           GetAddressableType(const string& parameter_absolute_name);
  Double*                     GetAddressable(const string& addressable_absolute_name);
  map<unsigned, Double>*      GetAddressableUMap(const string& addressable_absolute_name);
  OrderedMap<string, Double>* GetAddressableSMap(const string& addressable_absolute_name);
  vector<Double>*             GetAddressableVector(const string& addressable_absolute_name);

private:
  // methods
  Objects() = delete;
  explicit Objects(Model* model);
  virtual                     ~Objects() = default;
  base::Object*               FindObject(const string& parameter_absolute_name);
  void                        ExplodeString(const string& parameter_absolute_name, string &type, string& label, string& parameter, string& index);
  void                        ImplodeString(const string& type, const string& label, const string& parameter, const string& index, string& target_parameter);
  std::pair<string, string>   ExplodeParameterAndIndex(const string& parameter_absolute_name);

  // members
  Model*                      model_ = nullptr;
};

} /* namespace niwa */

#endif /* SOURCE_MODEL_OBJECTS_H_ */
