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
#include <boost/shared_ptr.hpp>

// Namespaces
namespace isam {
namespace parameters {

using std::string;
using std::vector;

/**
 * Class definition
 */
class Table {
public:
  explicit Table(const string &label);
  virtual                     ~Table() = default;
  void                        AddColumns(const vector<string> &columns);
  void                        AddRow(vector<string> &row);
  bool                        HasColumns() { return columns_.size() != 0; }
  unsigned                    GetColumnCount() { return columns_.size(); }

  // accessors
  void                        set_file_name(const string& file_name) { file_name_ = file_name; }
  string                      file_name() const { return file_name_; }
  void                        set_line_number(const unsigned& line_number) { line_number_ = line_number; }
  unsigned                    line_number() const { return line_number_; }
  const vector<string>&       columns() { return columns_; }
  vector<vector<string>>&     data() { return data_; }
  string                      location() const;

private:
  string                      label_ = "";
  string                      file_name_ = "";
  unsigned                    line_number_ = 0;
  vector<string>              columns_;
  vector<vector<string> >     data_;
};

/**
 * Typedefs
 */
typedef boost::shared_ptr<isam::parameters::Table> TablePtr;

} /* namespace parameters */
} /* namespace isam */
#endif /* TABLE_H_ */
