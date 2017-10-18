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

#include "Common/Categories/Categories.h"
#include "Common/Model/Model.h"
#include "Common/Utilities/String.h"
#include "Common/Utilities/To.h"
#include "Common/Translations/Translations.h"

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
void Table::AddRow(const vector<string> &row) {
  data_.push_back(row);
}

/**
 * Get the index for the specified column
 *
 * @param label The column label
 * @return The index for the label
 */
unsigned Table::column_index(const string& label) const {
  for (unsigned i = 0; i < columns_.size(); ++i) {
    if (columns_[i] == label)
      return i;
  }

  return columns_.size();
}
/**
 * Return a string that shows the location this parameter was defined.
 *
 * @return string containing the file and line details for this parameter
 */
string Table::location() const {
  string line_number;
  niwa::utilities::To<unsigned, string>(line_number_, line_number);
  return string("At line " + line_number + " in " + file_name_ + " the table " + label_ + " ");
}

/**
 *
 */
void Table::Populate(Model* model) {
  // get the index for PARAM_CATEGORY or PARAM_CATEGORIES if it exists
  unsigned category_index = column_index(PARAM_CATEGORY);
  category_index = category_index == columns_.size() ? column_index(PARAM_CATEGORIES) : category_index;
  if (category_index == columns_.size())
    return; // nothing to see here, move along

  // Make a copy of our data object so we can manipulate the container
  vector<vector<string>> data_copy = data_;
  data_.clear();

  vector<string> category_labels;
  string error = "";
  for (auto row : data_copy) {
    string category_lookup = row[category_index];
    category_labels = model->categories()->GetCategoryLabelsV(category_lookup, location());
    if (!utilities::String::HandleOperators(category_labels, error))
      LOG_FATAL() << location() << error;
    LOG_FINE() << "category_labels: " << boost::join(category_labels, " ");

    for (auto label : category_labels) {
      row[category_index] = label;
      LOG_FINE() << "re-adding row to table: " << boost::join(row, " ");
      data_.push_back(row);
    }
  }
}

} /* namespace parameters */
} /* namespace niwa */
