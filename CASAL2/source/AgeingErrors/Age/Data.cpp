/**
 * @file Data.cpp
 * @author C Marsh
 * @github https://github.com/Zaita
 * @date 01/09/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "Data.h"

#include "../../Utilities/To.h"

// namespaces
namespace niwa {
namespace ageingerrors {

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g., profiling, yields, projections, etc.)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate LaTeX for the documentation.
 */
Data::Data(shared_ptr<Model> model) : AgeingError(model) {
  data_table_ = new parameters::Table(PARAM_TABLE);

  parameters_.BindTable(PARAM_TABLE, data_table_, "The table of data specifying the ageing misclassification matrix", "", false);
}

/**
 * Destructor
 */
Data::~Data() {
  delete data_table_;
}

/**
 * Build the misspecification matrix
 */
void Data::DoBuild() {
  auto data = data_table_->data();
  if (data.size() != age_spread_) {
    LOG_ERROR_P(PARAM_TABLE) << "The number of rows provided " << data.size() << " does not match the age range of the model " << age_spread_;
    return;
  }
  if (data[0].size() != age_spread_) {
    LOG_ERROR_P(PARAM_TABLE) << "The number of columns provided " << data.size() << " does not match the age range of the model " << age_spread_;
    return;
  }

  Double value = 0.0;
  for (unsigned i = 0; i < data.size(); ++i) {
    for (unsigned j = 0; j < data[i].size(); ++j) {
      if (!utilities::To<string, Double>(data[i][j], value))
        LOG_ERROR_P(PARAM_TABLE) << "Could not convert the value " << data[i][j] << " to a Double";

      mis_matrix_[i][j] = value;
    }
  }
}

} /* namespace ageingerrors */
} /* namespace niwa */
