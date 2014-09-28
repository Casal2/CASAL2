/**
 * @file BaseObject.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is the highest level class in our object tree. All objects
 * that have to be created from a configuration block need to inherit from this class.
 *
 * This class creates access to some central global objects within the system
 * including: GlobalConfiguration, TheModel, ParameterList etc
 *
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef BASE_OBJECT_H_
#define BASE_OBJECT_H_

// Headers
#include <string>
#include <boost/shared_ptr.hpp>

#include "ParameterList/ParameterList.h"
#include "Translations/Translations.h"
#include "Utilities/NoCopy.h"
#include "Utilities/Types.h"

using isam::utilities::Double;

// Namespaces
namespace isam {

// Enumerated Types
namespace Estimable {
enum Type {
  kInvalid      = 0,
  kSingle       = 1,
  kVector       = 2,
  kStringMap    = 3,
  kUnsignedMap  = 4
};
}

namespace base {

using std::string;

/**
 * Class Definition
 */
class Object {
public:
  // Methods
  Object() = default;
  virtual                     ~Object() {};
  bool                        IsEstimableAVector(const string& label) const;
  unsigned                    GetEstimableSize(const string& label) const;
  Double*                     GetEstimable(const string& label);
  Double*                     GetEstimable(const string& label, const string& index);
  map<unsigned, Double>*      GetEstimableUMap(const string& label);
  map<string, Double>*        GetEstimableSMap(const string& label);
  vector<Double>*             GetEstimableVector(const string& label);
  Estimable::Type             GetEstimableType(const string& label) const;

  // Accessors and Mutators
  string                      label() const { return label_; }
  string                      type() const { return type_; }
  ParameterList&              parameters() { return parameters_; }
  void                        set_block_type(string value) { block_type_ = value; parameters_.set_parent_block_type(value); }
  void                        set_defined_file_name(string value) { parameters_.set_defined_file_name(value); }
  void                        set_defined_line_number(unsigned value) { parameters_.set_defined_line_number(value); }

protected:
  // Methods
  void                        RegisterAsEstimable(const string& label, Double* variable);
  void                        RegisterAsEstimable(const string& label, vector<Double>* variables);
  void                        RegisterAsEstimable(const string& label, map<string, Double>* variables);
  void                        RegisterAsEstimable(const string& label, map<unsigned, Double>* variables);

  // Accessors
  string                      location();

  // Members
  string                        block_type_           = "";
  string                        label_                = "";
  string                        type_                 = "";
  ParameterList                 parameters_;
  map<string, Double*>                  estimables_;
  map<string, vector<Double>* >         estimable_vectors_;
  map<string, map<string, Double>* >    estimable_s_maps_;
  map<string, map<unsigned, Double>* >  estimable_u_maps_;
  map<string, Estimable::Type>          estimable_types_;

  DISALLOW_COPY_AND_ASSIGN(Object);
};

/**
 * Typedef
 */
typedef boost::shared_ptr<isam::base::Object> ObjectPtr;


} /* namespace base */
} /* namespace isam */
#endif /* BASE_OBJECT_H_ */
