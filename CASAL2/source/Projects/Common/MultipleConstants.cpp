/**
 * @file MultipleConstants.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "MultipleConstants.h"

#include "../../Utilities/To.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Default constructor
 */
MultipleConstants::MultipleConstants(shared_ptr<Model> model) : Project(model) {
  data_table_ = new parameters::Table(PARAM_VALUES);
  parameters_.BindTable(PARAM_VALUES, data_table_, "Table of values for each -i input. Rows are -i value columns are for each year to project for.", "");
}
/**
 * Destructor
 */
MultipleConstants::~MultipleConstants() {
  delete data_table_;
}
/**
 * Validate
 */
void MultipleConstants::DoValidate() {
  
}

/**
 * Build
 */
void MultipleConstants::DoBuild() {
  // basic validation
  const vector<string>& columns = data_table_->columns();
  if (columns.size() != (years_.size()))
    LOG_FATAL_P(PARAM_VALUES) << " the input column count was " << columns.size() << ", but expected " << years_.size() << " columns. One for each projection year.";
  vector<vector<string>>& data = data_table_->data();
  LOG_FINE() << "-i count " << model_->get_addressable_values_count();
  if (data.size() != (model_->get_addressable_values_count())) 
    LOG_FATAL_P(PARAM_VALUES) << " the input row cound was " << data.size() << ", but expected " << model_->get_addressable_values_count() << " number of rows. This is each row for the -i or -I file plus the header.";
  // Check the first row is year followed by the years
  for(unsigned i = 1; i < columns.size(); ++i) {
    unsigned year = utilities::ToInline<string, unsigned>(columns[i]);
    // Check year is valid
    if (find(years_.begin(), years_.end(), year) == years_.end())
      LOG_FATAL_P(PARAM_VALUES) << "The first row should be a header with the projection years. Found '"<<year<<"' in the header, which isn't one of the projection years.";
  }
  projection_values_.resize(data.size());
  LOG_FINE() << "rows = " << data.size() << " cols " << columns.size();
  /**
   * Build our projected data values
   */
  unsigned counter = 0;
  for (vector<string> row : data) {
    for (unsigned i = 0; i < row.size(); ++i) {
      projection_values_[counter][years_[i]] = utilities::ToInline<string, Double>(row[i]);
    }
    counter++;
  }
}

/**
 * Reset
 */
void MultipleConstants::DoReset() {}

/**
 * Update
 */
void MultipleConstants::DoUpdate() {
  value_ = projection_values_[model_->get_current_addressable_value()][model_->current_year()] * multiplier_;
  LOG_FINE() << "Setting Value to: " << value_ << " dash -i index " << model_->get_current_addressable_value() << " year = " << model_->current_year();
  (this->*DoUpdateFunc_)(value_, true, model_->current_year());
}

} /* namespace projects */
} /* namespace niwa */
