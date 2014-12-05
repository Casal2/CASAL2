/**
 * @file Table.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Table.h"

#include "Utilities/To.h"

// Namespaces
namespace niwa {
namespace parameters {

/**
 * Default constructor
 */
Table::Table(const string &label)
: label_(label) {
}

/**
 * Add some columns to our table
 *
 * @param columns A list of columns for this table
 */
void Table::AddColumns(const vector<string> &columns) {
  columns_.assign(columns.begin(), columns.end());
}

/**
 * Add a row of data to our table
 *
 * @param row The row of data to add
 */
void Table::AddRow(vector<string> &row) {
  data_.push_back(row);
}

/**
 * Return a string that shows the location this parameter was defined.
 *
 * @return string containing the file and line details for this parameter
 */
string Table::location() const {
  string line_number;
  niwa::utilities::To<unsigned, string>(line_number_, line_number);
  return string("At line " + line_number + " of file " + file_name_);
}

} /* namespace parameters */
} /* namespace niwa */
