/**
 * @file CParameterList.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/02/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2011 - www.niwa.co.nz
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

#include "Parameter.h"
#include "Table.h"

// Namespaces
using std::map;
using std::vector;
using std::string;
using isam::parameterlist::TablePtr;
using isam::parameterlist::Parameter;
using isam::parameterlist::ParameterPtr;


namespace isam {

/**
 * Struct Definition: ParameterTable
 */
struct ParameterTable {
  string          file_name_;
  unsigned        line_number_;
  TablePtr        table_;
};

/**
 * Class Definition ParameterList
 */
class ParameterList {
public:
  // Methods
  ParameterList() = default;
  virtual                     ~ParameterList() = default;
  bool                        Add(const string& label, const string& value, const string& file_name, const unsigned& line_number);
  bool                        Add(const string& label, const vector<string>& values, const string& file_name, const unsigned& line_number);
  bool                        AddTable(const string& label, const vector<string>& columns, const vector<vector<string> >& data, const string& file_name, const unsigned& line_number);
  const ParameterPtr          Get(const string& label);
  void                        CopyFrom(const ParameterList& source);
  void                        Clear() { /*parameters_.clear(), tables_.clear();*/ }

  template<typename T>
  void                        Bind(const string& label, T* target, const string& description) {

  }

  template<typename T>
  void                        Bind(const string& label, T* target, const string& description, T default_value) {

  }

  template<typename T>
  void                        Bind(const string& label, vector<T>* target, const string& description, bool optional = false) {

  }


  void                        Populate() { };

  // Accessors
  string                      location(const string& label);
  const map<string, ParameterPtr>& parameters() { return parameters_; }

private:
  // Members
  map<string, ParameterPtr>         parameters_;
  map<string, ParameterTable>       tables_;
};

} /* namespace isam */

// Inline include for template methods
#include "ParameterList-inl.h"


#endif /* PARAMETER_LIST_H_ */
