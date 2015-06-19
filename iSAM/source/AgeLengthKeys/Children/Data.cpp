/**
 * @file Data.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 12/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Data.h"

#include <iostream>
#include <iomanip>

#include "Model/Model.h"
#include "LengthWeights/Manager.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace agelengthkeys {

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g profiling, yields, projections etc)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate Latex for the documentation.
 */
Data::Data() {
  data_table_ = TablePtr(new parameters::Table(PARAM_DATA));
  parameters_.BindTable(PARAM_DATA, data_table_, "Table of age length conversion table. Each row is length_bin age_1 age_n", "", false);
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void Data::DoBuild() {
  if (!data_table_)
    LOG_CODE_ERROR() << "!data_table_";

  unsigned length_bins_count = Model::Instance()->length_bins().size();

  /**
   * Build our data_by_year map so we can fill the gaps
   * and use it in the model
   */
  vector<vector<string>>& data = data_table_->data();
  for (vector<string> row : data) {
    if (row.size() != length_bins_count + 1) {
      LOG_ERROR_P(PARAM_DATA) << "row data count (" << row.size() << ") must be <age> <length> <length n> for a total of " << length_bins_count + 1 << " columns";
      return;
    }

    unsigned age = utilities::ToInline<string, unsigned>(row[0]);
    for (unsigned i = 1; i < row.size(); ++i) {
      lookup_table_[age][i - 1] = utilities::ToInline<string, Double>(row[i]);
    }
  }

  // rescale the data to ensure it's a proportion
  unsigned row_index = 0;
  for (auto row : lookup_table_) {
    ++row_index;
    Double total = 0.0;
    for (auto iter : row.second)
      total += iter.second;

    if (!utilities::doublecompare::IsOne(total)) {
      LOG_WARNING() << location() << "row " << row_index << " in data table sums to " << total << ". This should sum to 1.0. Auto re-scaling";
      for (auto& iter : row.second)
        iter.second /= total;
    }
  }
}

} /* namespace agelengthkeys */
} /* namespace niwa */
