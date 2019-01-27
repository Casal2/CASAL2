/**
 * @file Table.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class represents a table block loaded from the configuration file. It's
 * used for processing data and ensuring consistency of the table data when loaded
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef TABLE_H_
#define TABLE_H_

// Headers
#include <string>
#include <vector>
#include <map>
#include <memory>

#include "Utilities/Types.h"

// Namespaces
namespace niwa {
class Model;

namespace parameters {

using std::string;
using std::vector;
using std::map;
using niwa::utilities::Double;

/**
 * This class represents the rows from our table.
 * We use this so we can convert value types easier
 * and report on errors without having to know
 * a bunch of crap
 */
class Rows {
  typedef vector<string> DataType; // A single row



  DataType::iterator          Begin();
  DataType::iterator          End();
  unsigned                    Size();
};

/**
 * Class definition
 */
class Table {
public:
  explicit Table(const string &label);
  virtual                     ~Table() = default;
  void                        AddColumns(const vector<string> &columns);
  void                        AddRow(const vector<string> &row);
  bool                        HasColumns() { return columns_.size() != 0; }
  bool                        HasBeenDefined() const { return data_.size() != 0; }
  void                        Populate(Model* model);
  template<typename T>
  void                        CheckColumnValuesAreType(const string& column);
  template<typename T>
  void                        CheckColumnValuesContain(const string& column, const vector<T>& values);
  template<typename T>
  vector<T>                   GetColumnValuesAs(const string& column);

  // accessors
  void                        set_file_name(const string& file_name) { file_name_ = file_name; }
  string                      file_name() const { return file_name_; }
  void                        set_line_number(const unsigned& line_number) { line_number_ = line_number; }
  unsigned                    line_number() const { return line_number_; }
  unsigned                    row_count() const { return data_.size(); }
  unsigned                    column_count() const { return columns_.size(); }
  const vector<string>&       columns() { return columns_; }
  unsigned                    column_index(const string& label, bool throw_error = true) const;
  vector<vector<string>>&     data() { return data_; }
  string                      location() const;
  void                        set_is_optional(bool is_optional) { is_optional_ = is_optional; }
  bool                        is_optional() const { return is_optional_; }
  void                        set_requires_columns(bool requires_columns) { requires_columns_ = requires_columns; }
  bool                        requires_comlums() const { return requires_columns_; }
  void                        set_required_columns(const vector<string>& columns, bool allow_others = false);
  void                        set_optional_columns(const vector<string>& columns) { optional_columns_ = columns; allow_other_columns_ = true; }

private:
  string                      label_ = "";
  string                      file_name_ = "";
  unsigned                    line_number_ = 0;
  vector<string>              columns_;
  vector<vector<string> >     data_;
  bool                        is_optional_ = false;
  bool                        requires_columns_ = true;
  vector<string>              required_columns_;
  vector<string>              optional_columns_;
  bool                        allow_other_columns_ = false;
};
} /* namespace parameters */
} /* namespace niwa */

// headers
#include "Table-inl.h"

#endif /* TABLE_H_ */
