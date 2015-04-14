/**
 * @file Data.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 22/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Data.h"

#include <iostream>
#include <iomanip>

#include "Model/Model.h"
#include "SizeWeights/Manager.h"
#include "TimeSteps/Manager.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace agelengths {

using std::cout;
using std::endl;

/**
 * Default constructor
 */
Data::Data() {
  data_table_ = TablePtr(new parameters::Table(PARAM_DATA));

  parameters_.BindTable(PARAM_DATA, data_table_, "", "");
  parameters_.Bind<string>(PARAM_EXTERNAL_GAPS, &external_gaps_, "", "", PARAM_MEAN)
    ->set_allowed_values({PARAM_MEAN, PARAM_NEAREST_NEIGHBOUR});
  parameters_.Bind<string>(PARAM_INTERNAL_GAPS, &internal_gaps_, "", "", PARAM_MEAN)
    ->set_allowed_values({PARAM_MEAN, PARAM_NEAREST_NEIGHBOUR, PARAM_INTERPOLATE});
  parameters_.Bind<string>(PARAM_SIZE_WEIGHT, &size_weight_label_, "TBA", "");
  parameters_.Bind<Double>(PARAM_CV, &cv_, "TBA", "", 0.0);
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_, "TBA", "", PARAM_NORMAL);
}

/**
 *
 */
void Data::DoBuild() {
  size_weight_ = sizeweights::Manager::Instance().GetSizeWeight(size_weight_label_);
  if (!size_weight_)
    LOG_ERROR_P(PARAM_SIZE_WEIGHT) << "(" << size_weight_label_ << ") could not be found. Have you defined it?";

  if (!data_table_)
    LOG_CODE_ERROR() << "!data_table_";

  // basic validation
  const vector<string>& columns = data_table_->columns();
  if (columns.size() != Model::Instance()->age_spread() + 1)
    LOG_ERROR_P(PARAM_DATA) << "column count (" << columns.size() << ") must be <year> <ages> for a total of " << Model::Instance()->age_spread() + 1 << " columns";

  if (columns[0] != PARAM_YEAR)
    LOG_ERROR_P(PARAM_DATA) << "first column label must be 'year'. First column label was '" << columns[0] << "'";


  /**
   * Build our data_by_year map so we can fill the gaps
   * and use it in the model
   */
  vector<vector<string>>& data = data_table_->data();
  for (vector<string> row : data) {
    if (row.size() != columns.size())
      LOG_CODE_ERROR() << "row.size() != columns.size()";

    unsigned year = utilities::ToInline<string, unsigned>(row[0]);
    for (unsigned i = 1; i < row.size(); ++i)
      data_by_year_[year].push_back(utilities::ToInline<string, Double>(row[i]));
  }

  /**
   * Check if we're using a mean method and build a vector of means now
   * before we modify the data_by_year object by filling the external
   * gaps
   */
  if (external_gaps_ == PARAM_MEAN || internal_gaps_ == PARAM_MEAN) {
    unsigned age_spread = Model::Instance()->age_spread();
    Double total = 0.0;

    for (unsigned i = 0; i < age_spread; ++i) {
      total = 0.0;
      for (auto iter = data_by_year_.begin(); iter != data_by_year_.end(); ++iter)
        total += iter->second[i];
      means_.push_back(total / data_by_year_.size());
    }
  }

  // Fill our gaps
  FillExternalGaps();
  FillInternalGaps();
}

/**
 * Fill our external gaps in the data
 */
void Data::FillExternalGaps() {
  ModelPtr model = Model::Instance();
  vector<unsigned> model_years = model->years();
  vector<unsigned> missing_years;

  // do the filling
  if (external_gaps_ == PARAM_MEAN) {
    // find the missing years from the beginning
    for (unsigned year : model_years) {
      if (data_by_year_.find(year) == data_by_year_.end())
        missing_years.push_back(year);
      else
        break;
    }

    // find the missing years from the end
    for (auto year = model_years.rbegin(); year != model_years.rend(); ++year) {
      if (data_by_year_.find(*year) == data_by_year_.end())
        missing_years.push_back(*year);
      else
        break;
    }

    // loop over the ages
    for (unsigned year : missing_years)
      data_by_year_[year].assign(means_.begin(), means_.end());

  } else if (external_gaps_ == PARAM_NEAREST_NEIGHBOUR) {
    // find the missing years from the beginning
    for (unsigned year : model_years) {
      if (data_by_year_.find(year) == data_by_year_.end())
        missing_years.push_back(year);
      else
        break;
    }

    // loop over the ages
    auto iter = data_by_year_.begin();
    for (unsigned year : missing_years)
      data_by_year_[year].assign(iter->second.begin(), iter->second.end());

    // find the missing years from the end
    missing_years.clear();
    for (auto year = model_years.rbegin(); year != model_years.rend(); ++year) {
      if (data_by_year_.find(*year) == data_by_year_.end())
        missing_years.push_back(*year);
      else
        break;
    }

    // loop over the ages
    auto riter = data_by_year_.rbegin();
    for (unsigned year : missing_years)
      data_by_year_[year].assign(riter->second.begin(), riter->second.end());
  }
}

/**
 * Fill the internal missing years with one of the three
 * methods (mean, nearest_neighbour or interpolate)
 */
void Data::FillInternalGaps() {
  ModelPtr model = Model::Instance();
  vector<unsigned> model_years = model->years();

  // find the missing years
  vector<unsigned> missing_years;
  for (unsigned year : model_years) {
    if (data_by_year_.find(year) == data_by_year_.end())
      missing_years.push_back(year);
  }

  // do the filling
  if (internal_gaps_ == PARAM_MEAN) {
    // add the means to the data
    for (unsigned year : missing_years)
      data_by_year_[year].assign(means_.begin(), means_.end());

  } else if (internal_gaps_ == PARAM_NEAREST_NEIGHBOUR) {
    // use the nearest neighbour
    auto iter = data_by_year_.begin();
    ++iter;

    for (; iter != data_by_year_.end(); ++iter) {
      auto previous_iter = iter;
      --previous_iter;

      // if they're sequential skip this one
      if (iter->first - previous_iter->first == 1)
        continue;

      for (unsigned year = previous_iter->first + 1; year < iter->first; ++year) {
        if (year - previous_iter->first <= iter->first - year)
         data_by_year_[year].assign(previous_iter->second.begin(), previous_iter->second.end());
       else
         data_by_year_[year].assign(iter->second.begin(), iter->second.end());
      }
    }

  } else if (internal_gaps_ == PARAM_INTERPOLATE) {
    // interpolate between the two nearest neighbours
    auto iter = data_by_year_.begin();
    ++iter;

    for (; iter != data_by_year_.end(); ++iter) {
      auto previous_iter = iter;
      --previous_iter;

      // if they're sequential skip this one
      if (iter->first - previous_iter->first == 1)
        continue;

      unsigned steps = (iter->first - previous_iter->first) - 1;
      // build the interpolated values
      vector<Double> step_size;
      for (unsigned i = 0; i < iter->second.size(); ++i)
        step_size.push_back(((iter->second[i] - previous_iter->second[i]) / steps));

      unsigned steps_multiplier = 1;
      for (unsigned year = previous_iter->first + 1; year < iter->first; ++year, ++steps_multiplier) {
        for (unsigned i = 0; i < iter->second.size(); ++i)
          data_by_year_[year].push_back(previous_iter->second[i] + (step_size[i] * steps_multiplier));
      }
    }
  }
}

/**
 *
 */
Double Data::mean_length(unsigned year, unsigned age) {
  ModelPtr model = Model::Instance();
  Double current_value = data_by_year_.find(year)->second[age - model->min_age()];

  Double next_age      = current_value;
  if ((age+1) - model->min_age() < data_by_year_.find(year)->second.size())
    data_by_year_.find(year)->second[(age+1) - model->min_age()];

  Double proportion = time_step_proportions_[timesteps::Manager::Instance().current_time_step()];
  current_value += (next_age - current_value) * proportion;
  return current_value;
}

/**
 *
 */
Double Data::mean_weight(unsigned year, unsigned age) {
  Double size   = this->mean_length(year, age);
  return size_weight_->mean_weight(size, distribution_, cv_);
}

} /* namespace agelengths */
} /* namespace niwa */
