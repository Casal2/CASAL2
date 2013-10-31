/**
 * @file Parameter.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is an instance of a parameter stored in the parameter list.
 * It has accessors and mutators as well as a bunch of check functions
 * that allow us to quickly validate if the parameter is a valid one or not.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PARAMETER_H_
#define PARAMETER_H_

// Headers
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

// Namespaces
namespace isam {
namespace parameterlist {

using std::string;
using std::vector;

/**
 * Class Definition
 */
class Parameter {
public:
  // Methods
  Parameter(const string& label, const string& description);
  virtual                     ~Parameter() { };
  virtual void                Bind() = 0;
  void                        AddValue(const string& value);

  // Accessors
  void                        set_label(const string& label) { label_ = label; }
  void                        set_values(const vector<string>& values) { values_.assign(values.begin(), values.end()); }
  const vector<string>&       values() const { return values_; }
  void                        set_file_name(const string& file_name) { file_name_ = file_name; }
  string                      file_name() const { return file_name_; }
  void                        set_line_number(const unsigned& line_number) { line_number_ = line_number; }
  unsigned                    line_number() const { return line_number_; }
  void                        set_is_optional(bool is_optional) { is_optional_ = is_optional; }
  string                      location() const;

protected:
  // Methods
  template<typename T>
  void                        RequireValueType() const;

  // Members
  string                      label_ = "";
  string                      description_ = "";
  vector<string>              values_;
  string                      file_name_ = "";
  unsigned                    line_number_ = 0;
  bool                        is_optional_ = false;
};

// typedef
typedef boost::shared_ptr<Parameter> ParameterPtr;

} /* namespace parameterlist */
} /* namespace isam */

#include "Parameter-inl.h"

#endif /* PARAMETER_H_ */
