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

#include <algorithm>

#include "Utilities/String.h"
#include "Utilities/To.h"
#include "Translations/Translations.h"

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
  return string("At line " + line_number + " of file " + file_name_ + " in the table " + label_ + " ");
}

/**
 * This method will build a map containing the Category by ages
 */
void Table::FillMapOfCategoryAges(map<string, vector<Double>>& result) {
  if (columns_.size() < 2)
    LOG_CODE_ERROR() << "(columns_.size() < 2)";
  if (columns_[0] != PARAM_CATEGORY)
    LOG_ERROR() << location() << " must have " << PARAM_CATEGORY << " as the first column";

  vector<string> temp(columns_.begin()+1, columns_.end());
  vector<unsigned> age_list;
  vector<string> invalids = utilities::To<unsigned>(temp, age_list);
  if (invalids.size() != 0)
    LOG_ERROR() << location() << ". The following values could not be converted in to unsigned integers for ages: " << utilities::String::join<string>(invalids);

//  unsigned min_age = std::min_element(age_list.begin(), age_list.end());
//  unsigned max_age = std::max_element(age_list.begin(), age_list.end());

}

} /* namespace parameters */
} /* namespace niwa */
