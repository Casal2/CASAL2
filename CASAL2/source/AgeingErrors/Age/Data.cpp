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
#include "../../Utilities/math.h"

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
  parameters_.Bind<Double>(PARAM_TOLERANCE, &tol_, "Tolerance of the row sum check for the misclassification matrix table", "", 1e-5)->set_lower_bound(0.0, false);

  data_table_ = new parameters::Table(PARAM_DATA);
  parameters_.BindTable(PARAM_DATA, data_table_, "The table of data specifying the ageing misclassification matrix", "", false);
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
    LOG_ERROR_P(PARAM_DATA) << "The number of rows provided " << data.size() << " does not match the age range of the model " << age_spread_;
    return;
  }
  if (data[0].size() != age_spread_) {
    LOG_ERROR_P(PARAM_DATA) << "The number of columns provided " << data.size() << " does not match the age range of the model " << age_spread_;
    return;
  }

  Double value  = 0.0;
  Double RowSum = 0.0;

  for (unsigned i = 0; i < data.size(); ++i) {  // rows
    RowSum = 0.0;
    for (unsigned j = 0; j < data[i].size(); ++j) {  // columns
      if (!utilities::To<string, Double>(data[i][j], value))
        LOG_ERROR_P(PARAM_DATA) << "Could not convert the value " << data[i][j] << " to a Double";
      if (value < 0.0) {
        LOG_ERROR_P(PARAM_DATA) << ": The misclassification value (" << data[i][j] << ") cannot be less than 0.0";
      }
      RowSum += value;

      mis_matrix_[i][j] = value;
    }
    if (plus_group_) {
      if (!utilities::math::IsBasicallyEqual(RowSum, 1.0, tol_)) {
        LOG_ERROR_P(PARAM_DATA) << ": The sum of values in the misclassification matrix in row " << i + 1 << " was '" << RowSum << "'. These should sum to 1.0 ";
      }
    } else if (RowSum > (1.0 + tol_)) {
      LOG_ERROR_P(PARAM_DATA) << ": The sum of values in the misclassification matrix in row " << i + 1 << " was '" << RowSum << "'. These should be less than or equal to 1.0";
    }
  }
}

} /* namespace ageingerrors */
} /* namespace niwa */
