/**
 * @file Data.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 22/10/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "Data.h"

#include "../../Model/Managers.h"
#include "../../TimeSteps/Manager.h"
#include "../../Utilities/To.h"

// namespaces
namespace niwa {
namespace agelengths {

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
Data::Data(shared_ptr<Model> model) : AgeLength(model) {
  data_table_ = new parameters::Table(PARAM_DATA);

  parameters_.BindTable(PARAM_DATA, data_table_, "", "");
  parameters_.Bind<string>(PARAM_EXTERNAL_GAPS, &external_gaps_, "The method to use for external data gaps", "", PARAM_MEAN)
      ->set_allowed_values({PARAM_MEAN, PARAM_NEAREST_NEIGHBOUR});
  parameters_.Bind<string>(PARAM_INTERNAL_GAPS, &internal_gaps_, "The method to use for internal data gaps", "", PARAM_MEAN)
      ->set_allowed_values({PARAM_MEAN, PARAM_NEAREST_NEIGHBOUR, PARAM_INTERPOLATE});
  parameters_.Bind<string>(PARAM_TIME_STEP_MEASUREMENTS_WERE_MADE, &step_data_supplied_, "The time step label for which size-at-age data are provided", "");
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
  LOG_FINE() << "Building age length block " << label_;
  if (!data_table_)
    LOG_CODE_ERROR() << "!data_table_";
  if (model_->run_mode() == RunMode::kProjection)
    final_year_ = model_->projection_final_year();
  else
    final_year_ = model_->final_year();

  /**
   * create key parameters that are used to interpolate mean length between time_steps, relative to step_data_supplied_
   */
  TimeStep* time_step = model_->managers()->time_step()->GetTimeStep(step_data_supplied_);
  if (!time_step)
    LOG_ERROR_P(PARAM_TIME_STEP_MEASUREMENTS_WERE_MADE) << "could not find time_step " << step_data_supplied_;
  // Get time step index
  step_index_data_supplied_ = model_->managers()->time_step()->GetTimeStepIndex(step_data_supplied_);
  // Need to get ageing index
  unsigned time_step_index = 0;

  const vector<TimeStep*> ordered_time_steps = model_->managers()->time_step()->ordered_time_steps();
  for (auto time_step : ordered_time_steps) {
    for (auto process : time_step->processes()) {
      if (process->process_type() == ProcessType::kAgeing) {
        ageing_index_ = time_step_index;
      } else if (process->process_type() == ProcessType::kMortality) {
        mortality_index = time_step_index;
      }
    }
    // Find unique time-steps where the sizes need to be updated
    if (time_step_index == 0)
      steps_to_figure_.push_back(time_step_index);
    else if (time_step_index == ageing_index_)
      steps_to_figure_.push_back(time_step_index);
    else if (time_step_index == mortality_index)
      steps_to_figure_.push_back(time_step_index);
    else if (time_step_proportions_[time_step_index] != time_step_proportions_[time_step_index - 1])
      steps_to_figure_.push_back(time_step_index);

    ++time_step_index;
  }
  number_time_steps_ = ordered_time_steps.size();
  LOG_FINEST() << "number_time_steps_ " << number_time_steps_ << " steps_to_figure_.size() " << steps_to_figure_.size();

  // basic validation
  const vector<string>& columns = data_table_->columns();
  if (columns.size() != model_->age_spread() + 1)
    LOG_ERROR_P(PARAM_DATA) << "The column count (" << columns.size() << ") must be one column for each of <year> and <model ages> for a total of " << model_->age_spread() + 1
                            << " columns";
  if (columns[0] != PARAM_YEAR)
    LOG_ERROR_P(PARAM_DATA) << "The first column label must be 'year'. First column label was '" << columns[0] << "'";

  /**
   * Build our data_by_year map so we can fill the gaps
   * and use it in the model
   */
  vector<vector<string>>& data = data_table_->data();
  vector<Double>          total_length(model_->age_spread(), 0.0);
  Double                  number_of_years = 0.0;

  for (vector<string> row : data) {
    if (row.size() != columns.size())
      LOG_CODE_ERROR() << "row.size() != columns.size()";
    number_of_years += 1;
    if ((columns.size() - 1) != model_->age_spread())
      LOG_ERROR_P(PARAM_DATA) << "An age needs to be specified for every age in the model. " << columns.size() - 1 << " ages were specified, and there are " << model_->age_spread()
                              << " ages in the model";
    unsigned year = utilities::ToInline<string, unsigned>(row[0]);
    // Check year is valid
    if (find(model_->years().begin(), model_->years().end(), year) == model_->years().end())
      LOG_WARNING_P(PARAM_DATA) << "year " << year << " is not in the model run years, so this age length will not be used";

    for (unsigned i = 1; i < row.size(); ++i) {
      data_by_year_[year].push_back(utilities::ToInline<string, Double>(row[i]));
      total_length[i - 1] += utilities::ToInline<string, Double>(row[i]);
    }
  }

  /*
   * Build our average map for use in initialisation and simulation phases and projections (I am guessing, I haven't' tested projections yet)
   */
  for (unsigned i = 0; i < model_->age_spread(); ++i) {
    data_by_age_time_step_[step_index_data_supplied_][model_->min_age() + i] = total_length[i] / number_of_years;
  }
  // Make adjustment for data_by_age_time_step_ for different time steps.

  /**
   * Check if we're using a mean method and build a vector of means now
   * before we modify the data_by_year object by filling the external
   * gaps
   */
  // means_ is used in InterpolateTimeStepsForInitialConditions()
  for (unsigned i = 0; i < model_->age_spread(); ++i) {
    Double total = 0.0;
    for (auto iter = data_by_year_.begin(); iter != data_by_year_.end(); ++iter) total += iter->second[i];
    means_.push_back(total / data_by_year_.size());
  }

  // Do our timestep interpolation
  InterpolateTimeStepsForInitialConditions();

  // Fill our gaps
  FillExternalGaps();
  FillInternalGaps();

  // Initialise all time-steps and years with inputs.
  unsigned age;
  for (auto year : model_->years()) {
    LOG_FINEST() << "inital population for  = " << year;
    for (unsigned i = 0; i < number_time_steps_; ++i) {
      LOG_FINEST() << "step = " << i;
      LOG_FINEST() << "data_by_year_[" << year << "].size() " << data_by_year_[year].size();
      for (unsigned a = 0; a < model_->age_spread(); ++a) {
        age = a + model_->min_age();
        LOG_FINEST() << "a " << a << " age " << age;
        mean_length_by_year_[year][age][i] = data_by_year_[year][a];
      }
    }
  }
  // Do our timestep interpolation
  InterpolateTimeStepsForAllYears();
  change_mean_length_params_ = true;
}

/**
 * Interpolate across time steps for the object that is used in initialisation.
 */
void Data::InterpolateTimeStepsForInitialConditions() {
  LOG_TRACE();
  // Now we need to work with average_mean_sizes, using interpolation
  // We only have to figure sizes for steps j in steps_to_figure:
  unsigned a1, a2, age;
  Double   w1, w2;
  LOG_FINE() << "calculating initial age-length relationship for " << label_;
  for (unsigned i = 0; i < number_time_steps_; ++i) {
    LOG_FINE() << "time step = " << i;
    if (find(steps_to_figure_.begin(), steps_to_figure_.end(), i) != steps_to_figure_.end()) {
      LOG_FINE() << "adapting for time step = " << i << " initialiations";
      for (unsigned a = 0; a < model_->age_spread(); ++a) {
        age = a + model_->min_age();

        if ((step_index_data_supplied_ < i && i < ageing_index_) || (i < ageing_index_ && ageing_index_ < step_index_data_supplied_)
            || (ageing_index_ < step_index_data_supplied_ && step_index_data_supplied_ < i) || (ageing_index_ == step_index_data_supplied_) || (i == step_index_data_supplied_))
          a1 = a;
        else
          a1 = a - 1;

        a2 = a1 + 1;

        LOG_FINEST() << "a " << a << " a1 " << a1 << " a2 " << a2;

        if (a1 == a)
          w1 = 1 + time_step_proportions_[step_index_data_supplied_] - time_step_proportions_[i];
        else
          w1 = time_step_proportions_[step_index_data_supplied_] - time_step_proportions_[i];

        LOG_FINEST() << "w1 " << w1;

        w2 = 1.0 - w1;

        if ((a2 + model_->min_age()) > model_->max_age()) {
          a1 -= 1;
          a2 -= 1;
          w1 -= 1;
          w2 += 1;
        }

        LOG_FINEST() << "a1 " << a1 << " a2 " << a2 << " w1 " << w1 << " w2 " << w2;

        if ((a1 + model_->min_age()) < model_->min_age()) {
          a1 += 1;
          a2 += 1;
          w1 += 1;
          w2 -= 1;
        }

        LOG_FINEST() << "a1 " << a1 << " a2 " << a2 << " w1 " << w1 << " w2 " << w2;

        data_by_age_time_step_[i][age] = w1 * means_[a1] + w2 * means_[a2];
        LOG_FINEST() << "for age = " << a + model_->min_age() << " a1 = " << a1 + model_->min_age();
        LOG_FINEST() << means_[a1];
        LOG_FINEST() << "for a2 = " << a2 + model_->min_age();
        LOG_FINEST() << means_[a2];
        LOG_FINEST() << "w1 = " << w1 << " w2 = " << w2 << " final result";
        LOG_FINEST() << "data_by_age_time_step_[" << i << "][" << age << "] = " << data_by_age_time_step_[i][age];
      }
    }
  }
}

/**
 * Interpolate across time steps for the remaining years
 */
void Data::InterpolateTimeStepsForAllYears() {
  LOG_TRACE();
  // Now we need to work with average_mean_sizes, using interpolation
  // We only have to figure sizes for steps j in steps_to_figure:
  int      a1, a2;
  unsigned y1, y2, age;
  Double   w1, w2;
  for (auto year : model_->years()) {
    LOG_FINE() << "calculating age-length relationship for " << this->label() << " for year = " << year;
    for (unsigned i = 0; i < number_time_steps_; ++i) {
      LOG_FINE() << "time step = " << i;
      if (find(steps_to_figure_.begin(), steps_to_figure_.end(), i) != steps_to_figure_.end()) {
        LOG_FINE() << "adapting time step = " << i;
        for (int a = 0; a < (int)model_->age_spread(); ++a) {
          age = a + model_->min_age();

          y1 = (i >= step_index_data_supplied_ ? year : year - 1);
          y2 = y1 + 1;
          LOG_FINEST() << "year " << year << ": y1 = " << y1 << " y2 = " << y2;

          // havenet we already caught at line: 83
          if ((step_index_data_supplied_ < i && i < ageing_index_) || (i < ageing_index_ && ageing_index_ < step_index_data_supplied_)
              || (ageing_index_ < step_index_data_supplied_ && step_index_data_supplied_ < i) || (ageing_index_ == step_index_data_supplied_) || (i == step_index_data_supplied_))
            a1 = a;
          else
            a1 = a - 1;
          a2 = a1 + 1;
          LOG_FINEST() << "a " << a << ": a1 = " << a1 << " a2 = " << a2;

          if (a1 == a)
            w1 = 1 + time_step_proportions_[step_index_data_supplied_] - time_step_proportions_[i];
          else
            w1 = time_step_proportions_[step_index_data_supplied_] - time_step_proportions_[i];
          w2 = 1.0 - w1;
          LOG_FINEST() << "w2 = " << w2;

          if ((a2 + model_->min_age()) > model_->max_age() || y2 > final_year_) {
            a1 -= 1;
            a2 -= 1;
            if ((y1 - 1) > model_->start_year())
              y1 -= 1;  // Quick fix. Needs checking. Seems to work. Suggest fuller checking on case by case basis
            y2 -= 1;
            w1 -= 1;
            w2 += 1;
          }
          LOG_FINEST() << "a1 = " << a1 << ", a2 = " << a2 << ", w1 = " << w1 << ", w2 = " << w2 << ", y1 = " << y1 << ", y2 = " << y2;

          if ((a1 + model_->min_age()) < model_->min_age() || y1 < model_->start_year()) {
            LOG_FINEST() << "w2 = " << w2;
            a1 += 1;
            a2 += 1;
            y1 += 1;
            y2 += 1;
            w1 += 1;
            w2 -= 1;
          }
          LOG_FINEST() << "a1 = " << a1 << ", a2 = " << a2 << ", w1 = " << w1 << ", w2 = " << w2 << ", y1 = " << y1 << ", y2 = " << y2;

          if ((a2 + model_->min_age()) > model_->max_age() || y2 > final_year_ || (a1 + model_->min_age()) < model_->min_age() || y1 < model_->start_year()) {
            // unusual case - very little info on this cohort - just use averages
            LOG_FINEST() << "for age = " << a + model_->min_age();
            LOG_FINEST() << " final result";
            LOG_FINEST() << "mean_length_by_year_[" << year << "][" << age << "][" << i << "] = " << mean_length_by_year_[year][age][i];
            mean_length_by_year_[year][age][i] = data_by_age_time_step_[i][age];
          } else {
            LOG_FINEST() << "for age = " << a + model_->min_age() << " a1 = " << a1 + model_->min_age() << " y1 = " << y1 << " y2 = " << y2;
            LOG_FINEST() << data_by_year_[y1][a1];
            LOG_FINEST() << "for a2 = " << a2 + model_->min_age();
            LOG_FINEST() << data_by_year_[y2][a2];
            LOG_FINEST() << "w1 = " << w1 << " w2 = " << w2 << " final result";
            LOG_FINEST() << "mean_length_by_year_[" << year << "][" << age << "][" << i << "] = " << mean_length_by_year_[year][age][i];
            mean_length_by_year_[year][age][i] = w1 * data_by_year_[y1][a1] + w2 * data_by_year_[y2][a2];
          }
        }
      } else {
        // set equal to input first year
      }
    }
  }
}

/**
 * Fill the external gaps in the data
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

    LOG_FINE() << "missing years " << missing_years.size();

    // find the missing years from the end
    for (auto year = model_years.rbegin(); year != model_years.rend(); ++year) {
      if (data_by_year_.find(*year) == data_by_year_.end())
        missing_years.push_back(*year);
      else
        break;
    }

    LOG_FINE() << "missing years " << missing_years.size();

    // loop over the ages
    for (unsigned year : missing_years) data_by_year_[year].assign(means_.begin(), means_.end());

  } else if (external_gaps_ == PARAM_NEAREST_NEIGHBOUR) {
    // find the missing years from the beginning
    for (unsigned year : model_years) {
      if (data_by_year_.find(year) == data_by_year_.end())
        missing_years.push_back(year);
      else
        break;
    }

    LOG_FINE() << "missing years " << missing_years.size();

    // loop over the ages
    auto iter = data_by_year_.begin();
    for (unsigned year : missing_years) data_by_year_[year].assign(iter->second.begin(), iter->second.end());

    // find the missing years from the end
    missing_years.clear();
    for (auto year = model_years.rbegin(); year != model_years.rend(); ++year) {
      if (data_by_year_.find(*year) == data_by_year_.end())
        missing_years.push_back(*year);
      else
        break;
    }

    LOG_FINE() << "missing years " << missing_years.size();

    // loop over the ages
    auto riter = (data_by_year_.rbegin())->second;
    for (unsigned year : missing_years) data_by_year_[year].assign(riter.begin(), riter.end());
  }
}

/**
 * Fill the internal missing years with one of the three
 * methods (mean, nearest_neighbour, or interpolate)
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
    for (unsigned year : missing_years) data_by_year_[year].assign(means_.begin(), means_.end());

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
      for (unsigned i = 0; i < iter->second.size(); ++i) step_size.push_back(((iter->second[i] - previous_iter->second[i]) / steps));

      unsigned steps_multiplier = 1;
      for (unsigned year = previous_iter->first + 1; year < iter->first; ++year, ++steps_multiplier) {
        for (unsigned i = 0; i < iter->second.size(); ++i) data_by_year_[year].push_back(previous_iter->second[i] + (step_size[i] * steps_multiplier));
      }
    }
  }
}

/**
 * This is responsible for returning the correct mean length
 * for this class
 */
Double Data::calculate_mean_length(unsigned year, unsigned time_step, unsigned age) {
  if (model_->state() == State::kInitialise || model_->state() == State::kVerify) {
    LOG_FINEST() << "state " << model_->state();
    return data_by_age_time_step_[time_step][age];
  }
  return mean_length_by_year_[year][age][time_step];
}

} /* namespace agelengths */
} /* namespace niwa */
