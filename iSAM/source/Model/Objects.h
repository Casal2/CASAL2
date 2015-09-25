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
using niwa::utilities::Double;
using std::string;
using utilities::OrderedMap;

// classes
class Objects {
  friend class Model;
public:
  // methods
  Estimable::Type             GetEstimableType(const string& parameter_absolute_name, string& error);
  Double*                     GetEstimable(const string& label, string& error);
  map<unsigned, Double>*      GetEstimableUMap(const string& label, string& error);
  OrderedMap<string, Double>* GetEstimableSMap(const string& label, string& error);
  vector<Double>*             GetEstimableVector(const string& label, string& error);

  base::Object*               FindObject(const string& parameter_absolute_name, string& error);
  Double*                     FindEstimable(const string& estimable_absolute_name, string& error);

private:
  // methods
  void                        ExplodeString(const string& source_parameter, string &type, string& label, string& parameter, string& index);
  void                        ImplodeString(const string& type, const string& label, const string& parameter, const string& index, string& target_parameter);

  Objects() = default;
  virtual                     ~Objects() = default;

  // members
  Model*                      model_ = nullptr;
};

} /* namespace niwa */

#endif /* SOURCE_MODEL_OBJECTS_H_ */
