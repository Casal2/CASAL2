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

#include "Categories/Categories.h"
#include "Model/Model.h"
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
void Table::AddRow(const vector<string> &row) {
  data_.push_back(row);
}

/**
 * Get the index for the specified column
 *
 * @param label The column label
 * @return The index for the label
 */
unsigned Table::column_index(const string& label, bool throw_error) const {
  for (unsigned i = 0; i < columns_.size(); ++i) {
    if (columns_[i] == label)
      return i;
  }

  if (throw_error)
    LOG_FATAL() << location() << "could not find column " << label << " in the table definition";

  return columns_.size();
}

/**
 * This method will set a list of required columns. This will be used during the Populate method
 * to validate the columns exist and the table structure is as expected.
 */
void Table::set_required_columns(const vector<string>& columns, bool allow_others) {
  required_columns_ = columns;
  allow_other_columns_ = allow_others;
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
 * The populate method works as a validate method and a data container
 */
void Table::Populate(Model* model) {
  if (model == nullptr)
    LOG_CODE_ERROR() << "model == nullptr";

  /**
   * Check the required columns if we've specified any.
   */
  if (required_columns_.size() > 0) {
    vector<string> missing_columns;
    for (const string& column : required_columns_) {
      if (column_index(column, false) == columns_.size())
        missing_columns.push_back(column);
    }
    if (missing_columns.size() > 0)
      LOG_ERROR() << location() << " is missing the following column headers: " << boost::join(missing_columns, ", ");

    // See if we have any extra columns that are not allowed.
    if (required_columns_.size() != columns_.size() && !allow_other_columns_) {
      vector<string> extra_columns;
      for (const string& column : columns_) {
        if (std::find(required_columns_.begin(), required_columns_.end(), column) == required_columns_.end()
            && std::find(optional_columns_.begin(), optional_columns_.end(), column) == optional_columns_.end())
          extra_columns.push_back(column);
      }

      if (extra_columns.size() > 0)
        LOG_ERROR() << location() << "has extra columns not allowed in the definition: " << boost::join(extra_columns, ", ");
    }
  }

  // get the index for PARAM_CATEGORY or PARAM_CATEGORIES if it exists
  unsigned category_index = column_index(PARAM_CATEGORY, false);
  category_index = category_index == columns_.size() ? column_index(PARAM_CATEGORIES, false) : category_index;
  if (category_index != columns_.size()) {
    // Make a copy of our data object so we can manipulate the container
    vector<vector<string>> data_copy = data_;
    data_.clear();

    /**
     * This code will handle category shorthand while verifying the categories actually
     * exist. So something like male,female.2000 will check male.2000 and female.2000.
     * This code should probably be handled elsewhere?
     */
    vector<string> category_labels;
    string error = "";
    for (auto row : data_copy) {
      string category_lookup = row[category_index];
      category_labels = model->categories()->GetCategoryLabelsV(category_lookup, location());
      if (!utilities::String::HandleOperators(category_labels, error))
        LOG_FATAL() << location() << error;
      LOG_FINE() << "category_labels: " << boost::join(category_labels, " ");

      for (auto label : category_labels) {
        if (!model->categories()->IsValid(label)) {
          LOG_ERROR() << location() << "contains an invalid category " << label << ". Perhaps you mis-typed the short-hand?";
        }

        row[category_index] = label;
        LOG_FINE() << "re-adding row to table: " << boost::join(row, " ");
        data_.push_back(row);
      }
    }
  } // if (category_index != columns_.size()) {

  /**
   * This code will check for any columns called year or years and if one is found
   * it'll check that any values in this column fall within the years for this model
   */
  unsigned year_index = column_index(PARAM_YEAR, false);
  year_index = year_index == columns_.size() ? column_index(PARAM_YEARS, false) : year_index;
  if (year_index != columns_.size()) {
    vector<unsigned> years = model->years_all();

    if (column_index(PARAM_YEAR, false) != columns_.size())
      CheckColumnValuesContain<unsigned>(PARAM_YEAR, years);
    else
      CheckColumnValuesContain<unsigned>(PARAM_YEARS, years);
  } // if (year_index != columns_.size()) {

}

} /* namespace parameters */
} /* namespace niwa */
