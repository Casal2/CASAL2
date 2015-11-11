/**
 * @file BaseObject.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is the highest level class in our object tree. All objects
 * that have to be created from a configuration block need to inherit from this class.
 *
 * This class creates access to some central global objects within the system
 * including: GlobalConfiguration, TheModel, ParameterList etc
 *
 */
#ifndef BASE_OBJECT_H_
#define BASE_OBJECT_H_

// Headers
#include <string>
#include <memory>

#include "Logging/Logging.h"
#include "ParameterList/ParameterList.h"
#include "Translations/Translations.h"
#include "Utilities/Map.h"
#include "Utilities/NoCopy.h"
#include "Utilities/Types.h"

using niwa::utilities::Double;

// Namespaces
namespace niwa {

// Enumerated Types
namespace Estimable {
enum Type {
  kInvalid      = 0,
  kSingle       = 1,
  kVector       = 2,
  kStringMap    = 3,
  kUnsignedMap  = 4,
  kVectorStringMap = 5
};
}

namespace base {

using std::string;
using utilities::OrderedMap;
using std::vector;
using std::map;

// classes
class Object {
public:
  // Methods
  Object() = default;
  virtual                         ~Object() {};
  bool                            HasEstimable(const string& label) const;
  bool                            IsEstimableAVector(const string& label) const;
  unsigned                        GetEstimableSize(const string& label) const;
  Double*                         GetEstimable(const string& label);
  Double*                         GetEstimable(const string& label, const string& index);
  map<unsigned, Double>*          GetEstimableUMap(const string& label);
  OrderedMap<string, Double>*     GetEstimableSMap(const string& label);
  vector<Double>*                 GetEstimableVector(const string& label);
  Estimable::Type                 GetEstimableType(const string& label) const;
  void                            PrintParameterQueryInfo();

  // pure virtual methods
  virtual void                    Reset() = 0;

  // Accessors and Mutators
  string                      label() const { return label_; }
  string                      type() const { return type_; }
  ParameterList&              parameters() { return parameters_; }
  string                      location();
  void                        set_block_type(string value) { block_type_ = value; parameters_.set_parent_block_type(value); }
  void                        set_defined_file_name(string value) { parameters_.set_defined_file_name(value); }
  void                        set_defined_line_number(unsigned value) { parameters_.set_defined_line_number(value); }

protected:
  // Methods
  void                        RegisterAsEstimable(const string& label, Double* variable);
  void                        RegisterAsEstimable(const string& label, vector<Double>* variables);
  void                        RegisterAsEstimable(const string& label, OrderedMap<string, Double>* variables);
  void                        RegisterAsEstimable(const string& label, map<unsigned, Double>* variables);
  void                        RegisterAsEstimable(map<string, vector<Double>>* variables);

  // Members
  string                        block_type_           = "";
  string                        label_                = "";
  string                        type_                 = "";
  ParameterList                 parameters_;
  map<string, Double*>          estimables_;
  map<string, vector<Double>* > estimable_vectors_;
  map<string, Estimable::Type>  estimable_types_;

  map<string, map<unsigned, Double>* >      estimable_u_maps_;
  map<string, OrderedMap<string, Double>* > estimable_s_maps_;
  vector<map<string, vector<Double>>* >     unnamed_estimable_s_map_vector_;

  DISALLOW_COPY_AND_ASSIGN(Object);
};

} /* namespace base */
} /* namespace niwa */
#endif /* BASE_OBJECT_H_ */
