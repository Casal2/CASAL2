/**
 * @file CParameterList.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/02/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2011 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Note: This file was originally developed in 2009, but has had some
 * revisions.
 *
 * The parameter list is an object container that stores and retreives
 * parameters for the parent object. This object is used by every object
 * that needs to have configuration information loaded from a config file.
 *
 * All information from the configuration file is passed to the
 * ParameterList for validation (CheckValid()) and retrieval.
 *
 * It supports a standard parameter paradigm of having a parameter name
 * and 0-n values stored against it.
 *
 * If you specify the same parameter on multiple lines it will concatenate
 * all of the values into a single parameter with n values.
 * (e.g below, param1 would be a single parameter with 3 values)
 *
 * Standard Configuration looks something like
 * @<section/object type> <label>
 * <param1> <value1>
 * <param1> <value2> <value3>
 * <param2> <value1>
 *
 * It also supports a table construct for defining tables of information.
 *
 * A significant difference between table definitions and parameter
 * definitions is that tables will not be concatenated. If you specify
 * 2 tables with the same name they will be considered different objects.
 *
 * A table definition looks something like:
 * table <name>
 * <row1> <row2> <rowN>
 * <value1> <value2> <value3>
 * <valueN> <valueN>
 * <valueN>
 * end_table
 *
 * The values stored in the table can be input in any way you like, they do
 * not have to match to rows during definition. They will be organised
 * internally.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PARAMETER_LIST_H_
#define PARAMETER_LIST_H_

// Headers
#include <map>
#include <vector>
#include <string>
#include <set>
#include <memory>

#include "Parameter.h"
#include "Table.h"
#include "Parameters/Bindable.h"
#include "Parameters/BindableVector.h"

// Namespaces
namespace niwa {

using std::map;
using std::vector;
using std::string;
using niwa::parameters::Table;
using niwa::parameters::Bindable;
using niwa::parameters::BindableVector;
using niwa::parameterlist::Parameter;
using niwa::parameterlist::Parameter;
class Object;

/**
 * Class Definition ParameterList
 */
class ParameterList {
public:
  // Methods
  ParameterList() = default;
  virtual                     ~ParameterList();
  bool                        Add(const string& label, const string& value, const string& file_name, const unsigned& line_number);
  bool                        Add(const string& label, const vector<string>& values, const string& file_name, const unsigned& line_number);
  Parameter*                  Get(const string& label);
  parameters::Table*          GetTable(const string& label);
  void                        CopyFrom(const ParameterList& source, string parameter_label);
  void                        CopyFrom(const ParameterList& source, string parameter_label, const unsigned &value_index);
  void                        Clear();

  template<typename T>
  Bindable<T>*                Bind(const string& label, T* target, const string& description, const string& values);
  template<typename T>
  Bindable<T>*                Bind(const string& label, T* target, const string& description, const string& values, T default_value);
  template<typename T>
  BindableVector<T>*          Bind(const string& label, vector<T>* target, const string& description, const string& values, bool optional = false);

  void                        BindTable(const string& label, parameters::Table* table, const string& description,
                                const string& values, bool requires_columns = true, bool optional = false);
  void                        Populate();

  // accessors
  string                      location(const string& label);
  void                        set_parent_block_type(const string& block_type) { parent_block_type_ = block_type; }
  void                        set_defined_file_name(string value) { defined_file_name_ = value; }
  string                      defined_file_name() const { return defined_file_name_; }
  void                        set_defined_line_number(unsigned value) { defined_line_number_ = value; }
  unsigned                    defined_line_number() const { return defined_line_number_; }
  map<string, Parameter*>&    parameters() { return parameters_; }
  void                        set_ignore_all_parameters() { ignore_all_parameters_ = true; }
  bool                        ignore_all_parameters() { return ignore_all_parameters_; }
  bool                        has_been_populated() const { return already_populated_; }

private:
  // members
  bool                        already_populated_ = false;
  string                      parent_block_type_    = "<unknown>";
  string                      defined_file_name_    = "<unknown>";
  unsigned                    defined_line_number_  = 0;
  map<string, Parameter*>     parameters_;
  map<string, Table*>         tables_;
  bool                        ignore_all_parameters_ = false;
};

} /* namespace niwa */

// Inline include for template methods
#include "ParameterList-inl.h"


#endif /* PARAMETER_LIST_H_ */
