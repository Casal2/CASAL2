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
#ifndef CPARAMETERLIST_H_
#define CPARAMETERLIST_H_

// Headers
#include <map>
#include <vector>
#include <string>
#include <set>

// Namespaces
using std::map;
using std::vector;
using std::string;
using std::set;

namespace isam {

/**
 * Struct Definition: SParameterTable
 */
struct ParameterTable {
  vector<string> columns_;
  vector<string> data_;
};

/**
 * Class Definition ParameterList
 */
class ParameterList {
public:
  // Methods
  ParameterList();
  virtual                     ~ParameterList();
  void                        registerAllowed(string name);
  void                        addParameter(string name, string value);
  void                        setParameter(string name, string value);
  void                        addTable(string name, vector<string> &columns, vector<string> &data);
  void                        replaceTable(string tableName, vector<string> &columns, vector<string> &data);
  bool                        hasParameter(string name, bool checkTables = true);
  unsigned                    countTables(string name);
  void                        checkInvalidParameters(bool skipTables = false);
  vector<string>              getTableNames();
  void                        clear();

  // Accessors and Mutators
  string                      getString(string name, bool optional = false, string defaultValue = "");
  double                      getDouble(string name, bool optional = false, double defaultValue = 0.0);
  int                         getInt(string name, bool optional = false, int defaultValue = 0);
  unsigned                    getUnsigned(string name, bool optional = false, unsigned defaultValue = 0);
  bool                        getBool(string name, bool optional = false, bool defaultValue = true);
  void                        fillVector(vector<string> &list, string name, bool optional = false);
  void                        fillVector(vector<double> &list, string name, bool optional = false);
  void                        fillVector(vector<int> &list, string name, bool optional = false);
  void                        fillVector(vector<bool> &list, string name, bool optional = false);
  template<class Type> void   fillVector(vector<Type> &list, string name, bool optional = false);

  // Table getters
  void                        fillTableColumnVector(vector<string> &list, string tableName, unsigned tableIndex = 0);
  void                        fillTableDataVector(vector<string> &list, string tableName, unsigned tableIndex = 0);

protected:
  // Members
  map<string, vector<string> >          parameters_;
  vector<string>                        valid_parameters_;
  map<string, vector<ParameterTable> >  tables_;

};

} /* namespace isam */

// Inline include for template methods
#include "ParameterList-inl.h"


#endif /* CPARAMETERLIST_H_ */
