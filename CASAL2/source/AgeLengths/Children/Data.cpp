/**
 * @file Data.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 22/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "Data.h"

#include "LengthWeights/Manager.h"
#include "Model/Managers.h"
#include "TimeSteps/Manager.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace agelengths {

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
Data::Data(Model* model) : AgeLength(model) {
  data_table_ = new parameters::Table(PARAM_DATA);

  parameters_.BindTable(PARAM_DATA, data_table_, "", "");
  parameters_.Bind<string>(PARAM_EXTERNAL_GAPS, &external_gaps_, "", "", PARAM_MEAN)->set_allowed_values({PARAM_MEAN, PARAM_NEAREST_NEIGHBOUR});
  parameters_.Bind<string>(PARAM_INTERNAL_GAPS, &internal_gaps_, "", "", PARAM_MEAN)->set_allowed_values({PARAM_MEAN, PARAM_NEAREST_NEIGHBOUR, PARAM_INTERPOLATE});
  parameters_.Bind<string>(PARAM_LENGTH_WEIGHT, &length_weight_label_, "TBA", "");
  parameters_.Bind<bool>(PARAM_BY_LENGTH, &by_length_, "Specifies if the linear interpolation of CV's is a linear function of mean length at age. Default is just by age", "", true);

}

/**
 * Destructor
 */
Data::~Data() {
  delete data_table_;
}
/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void Data::DoBuild() {
  length_weight_ = model_->managers().length_weight()->GetLengthWeight(length_weight_label_);
  if (!length_weight_)
    LOG_ERROR_P(PARAM_LENGTH_WEIGHT) << "(" << length_weight_label_ << ") could not be found. Have you defined it?";
  if (!data_table_)
    LOG_CODE_ERROR() << "!data_table_";

  // basic validation
  const vector<string>& columns = data_table_->columns();
  if (columns.size() != model_->age_spread() + 1)
    LOG_ERROR_P(PARAM_DATA) << "column count (" << columns.size() << ") must be <year> <ages> for a total of " << model_->age_spread() + 1 << " columns";
  if (columns[0] != PARAM_YEAR)
    LOG_ERROR_P(PARAM_DATA) << "first column label must be 'year'. First column label was '" << columns[0] << "'";

  /**
   * Build our data_by_year map so we can fill the gaps
   * and use it in the model
   */
  vector<vector<string>>& data = data_table_->data();
  vector<Double> total_length(model_->age_spread(), 0.0);
  Double number_of_years = 0.0;

  for (vector<string> row : data) {
    if (row.size() != columns.size())
      LOG_CODE_ERROR() << "row.size() != columns.size()";
    ++number_of_years;
    unsigned year = utilities::ToInline<string, unsigned>(row[0]);
    for (unsigned i = 1; i < row.size(); ++i) {
      data_by_year_[year].push_back(utilities::ToInline<string, Double>(row[i]));
      total_length[i - 1] += utilities::ToInline<string, Double>(row[i]);
    }
  }

  /*
   * Build our average map for use in initialisation and simulation phases
   */
  for (unsigned i = 0; i < model_->age_spread(); ++i)
    data_by_age_[model_->min_age() + i] = total_length[i] / number_of_years;

  /**
   * Check if we're using a mean method and build a vector of means now
   * before we modify the data_by_year object by filling the external
   * gaps
   */
  if (external_gaps_ == PARAM_MEAN || internal_gaps_ == PARAM_MEAN) {
    for (unsigned i = 0; i < model_->age_spread(); ++i) {
      Double total = 0.0;
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
  vector<unsigned> model_years = model_->years();
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
  vector<unsigned> model_years = model_->years();

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
 * Get the mean length of a single population
 *
 * @param year The year we want mean length for
 * @param age The age of the population we want mean length for
 * @return The mean length for 1 member
 */
Double Data::mean_length(unsigned year, unsigned age) {
  if (model_->state() == State::kInitialise) // need to add something here for when we are simulating
     return data_by_age_[age];

  Double current_value = data_by_year_.find(year)->second[age - model_->min_age()];

  Double next_age      = current_value;
  if ((age+1) - model_->min_age() < data_by_year_.find(year)->second.size())
    data_by_year_.find(year)->second[(age+1) - model_->min_age()];

  Double proportion = time_step_proportions_[model_->managers().time_step()->current_time_step()];
  current_value += (next_age - current_value) * proportion;
  return current_value;
}

/**
 * Get the mean weight of a single population
 *
 * @param year The year we want mean weight for
 * @param age The age of the population we want mean weight for
 * @return mean weight for 1 member cvs_[i]
 */
Double Data::mean_weight(unsigned year, unsigned age) {
  Double size   = this->mean_length(year, age);
  unsigned time_step = model_->managers().time_step()->current_time_step();
  return length_weight_->mean_weight(size, distribution_, cvs_[year][age][time_step]);
}

/*
 * Create a 3d look up map of CV's that gets used in mean_weight and any distribution around
 * converting age to length
 */
void Data::BuildCV() {
  LOG_TRACE();
  unsigned min_age = model_->min_age();
  unsigned max_age = model_->max_age();
  unsigned start_year = model_->start_year();
  unsigned final_year = model_->final_year();
  vector<string> time_steps = model_->time_steps();

  for (unsigned year_iter = start_year; year_iter <= final_year; ++year_iter) {
    for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter) {
      if (cv_last_ == 0.0) { // A test that is robust... If cv_last_ is not in the input then assume cv_first_ represents the cv for all age classes i.e constant cv
        for (unsigned age_iter = min_age; age_iter <= max_age; ++age_iter)
          cvs_[year_iter][age_iter][step_iter] = (cv_first_);

      } else if (by_length_) {  // if passed the first test we have a min and max CV. So ask if this is interpolated by length at age
        for (unsigned age_iter = min_age; age_iter <= max_age; ++age_iter)
          cvs_[year_iter][age_iter][step_iter] = ((mean_length(year_iter, age_iter) - mean_length(year_iter, min_age)) * (cv_last_ - cv_first_)
              / (mean_length(year_iter, max_age) - mean_length(year_iter, min_age)) + cv_first_);

      } else {
        // else Do linear interpolation between cv_first_ and cv_last_ based on age class
        for (unsigned age_iter = min_age; age_iter <= max_age; ++age_iter) {
          cvs_[year_iter][age_iter][step_iter] = (cv_first_ + (cv_last_ - cv_first_) * (age_iter - min_age) / (max_age - min_age));
        }
      }
    }
  }
}

} /* namespace agelengths */
} /* namespace niwa */
