/**
 * @file AgeLength.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 23/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// defines
#define _USE_MATH_DEFINES

// headers
#include "AgeLength.h"

#include <cmath>

#include "../Estimates/Manager.h"
#include "../LengthWeights/LengthWeight.h"
#include "../LengthWeights/Manager.h"
#include "../Model/Managers.h"
#include "../TimeSteps/Manager.h"
#include "../Utilities/Map.h"

// namespaces
namespace niwa {

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
AgeLength::AgeLength(shared_ptr<Model> model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label of the age length relationship", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of age length relationship", "");
  parameters_
      .Bind<Double>(
          PARAM_TIME_STEP_PROPORTIONS, &time_step_proportions_,
          "The fraction of the year applied in each time step that is added to the age for the purposes of evaluating the length, i.e., a value of 0.5 for a time step will evaluate the length of individuals at age+0.5 in that time step",
          "", true)
      ->set_range(0.0, 1.0);
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_label_, "The assumed distribution for the growth curve", "", PARAM_NORMAL);
  parameters_.Bind<Double>(PARAM_CV_FIRST, &cv_first_, "The CV for the first age class", "", Double(0.0))->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_CV_LAST, &cv_last_, "The CV for last age class", "", Double(0.0))->set_lower_bound(0.0);
  parameters_
      .Bind<string>(PARAM_COMPATIBILITY, &compatibility_,
                    "Backwards compatibility option: either casal2 (the default) or casal to use the (less accurate) cumulative normal function from CASAL", "", PARAM_CASAL2)
      ->set_allowed_values({PARAM_CASAL, PARAM_CASAL2});
  parameters_.Bind<bool>(PARAM_BY_LENGTH, &by_length_, "Specifies if the linear interpolation of CVs is a linear function of mean length at age. Default is by age only", "", true);
  parameters_.Bind<string>(PARAM_LENGTH_WEIGHT, &length_weight_label_, "The label from an associated length-weight block", "");

  RegisterAsAddressable(PARAM_CV_FIRST, &cv_first_);
  RegisterAsAddressable(PARAM_CV_LAST, &cv_last_);
}

/**
 * Populate any parameters,
 * Validate that values are within expected ranges when bind<>() overloads cannot be used
 *
 * Note: all parameters are populated from configuration files
 */
void AgeLength::Validate() {
  LOG_FINEST() << "by_length_ = " << by_length_;
  parameters_.Populate(model_);

  // get offsets
  year_offset_ = model_->start_year();
  age_offset_  = model_->min_age();

  if (distribution_label_ == PARAM_NORMAL)
    distribution_ = Distribution::kNormal;
  else if (distribution_label_ == PARAM_LOGNORMAL)
    distribution_ = Distribution::kLogNormal;
  else if (distribution_label_ == PARAM_NONE)
    distribution_ = Distribution::kNone;
  else
    LOG_ERROR() << "The age-length distribution '" << distribution_label_ << "' is not valid.";

  DoValidate();
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void AgeLength::Build() {
  LOG_FINE() << "Build()";
  unsigned time_step_count = model_->time_steps().size();
  if (time_step_proportions_.size() == 0) {
    time_step_proportions_.assign(time_step_count, 0.0);

  } else if (time_step_proportions_.size() == 1 && time_step_proportions_.size() != time_step_count) {
    Double temp = time_step_proportions_[0];
    time_step_proportions_.assign(time_step_count, temp);

  } else if (time_step_count != time_step_proportions_.size()) {
    LOG_ERROR_P(PARAM_TIME_STEP_PROPORTIONS) << "size (" << time_step_proportions_.size() << ") must match the number "
                                             << "of defined time steps for this process (" << time_step_count << ")";
  }

  for (auto iter : time_step_proportions_) {
    if (iter < 0.0 || iter > 1.0)
      LOG_ERROR_P(PARAM_TIME_STEP_PROPORTIONS) << "Time step proportion value (" << iter << ") must be in the range 0.0 to 1.0 inclusive";
  }
  // Get length weight pointer
  length_weight_ = model_->managers()->length_weight()->GetLengthWeight(length_weight_label_);
  if (!length_weight_)
    LOG_ERROR_P(PARAM_LENGTH_WEIGHT) << "Length-weight label " << length_weight_label_ << " was not found";
  // Subscribe this class to this length_weight class
  // This creates a dependency, for the timevarying functionality. See NotifySubscribers() called in TimeVarying.cpp line 107:108
  length_weight_->SubscribeToRebuildCache(this);
  // if this length weight class time-varys then it will flag this age-length class to rebuild cache

  // allocate memory for cvs
  cvs_.resize(model_->years().size());
  for (unsigned year_ndx = 0; year_ndx < cvs_.size(); ++year_ndx) {
    cvs_[year_ndx].resize(time_step_count);
    for (unsigned time_step_ndx = 0; time_step_ndx < cvs_[year_ndx].size(); ++time_step_ndx) {
      cvs_[year_ndx][time_step_ndx].resize(model_->age_spread(), 0.0);
    }
  }
  // allocate memory for mean weight and length
  mean_length_by_timestep_age_.resize(time_step_count);
  mean_weight_by_timestep_age_.resize(time_step_count);
  for (unsigned time_step_ndx = 0; time_step_ndx < mean_weight_by_timestep_age_.size(); ++time_step_ndx) {
    mean_length_by_timestep_age_[time_step_ndx].resize(model_->age_spread(), 0.0);
    mean_weight_by_timestep_age_[time_step_ndx].resize(model_->age_spread(), 0.0);
  }
  // age-lenght matrix assumes all down stream classes i.e. processes and observations have flagged BuildAgeLengthMatrixForTheseYears
  LOG_FINE() << "years we need to build age-length matrix for = " << age_length_matrix_years_.size();
  if(age_length_matrix_years_.size() > 0) {
    age_length_transition_matrix_.resize(age_length_matrix_years_.size());
    for(unsigned year_iter = 0; year_iter < age_length_matrix_years_.size(); ++year_iter) {
      age_length_transition_matrix_[year_iter].resize(time_step_count);
      for (unsigned time_step_ndx = 0; time_step_ndx < time_step_count; ++time_step_ndx) {
        age_length_transition_matrix_[year_iter][time_step_ndx].resize(model_->age_spread());
        numbers_by_age_length_transition_.resize(model_->age_spread());
        for(unsigned age_iter = 0; age_iter < model_->age_spread(); ++age_iter) {
          age_length_transition_matrix_[year_iter][time_step_ndx][age_iter].resize(model_->get_number_of_length_bins(), 0.0);
          numbers_by_age_length_transition_[age_iter].resize(model_->get_number_of_length_bins(), 0.0);
        }
      }
    }
  }

  model_years_ = model_->years();
  // Build child dependencies
  DoBuild();
  // 
  PopulateCV();
  //
  UpdateYearContainers();
  // Build Age-length matrix
  PopulateAgeLengthMatrix();
}

/**
 * Populates the CVs, memory has been allocated in the Build()
 * populates a 3-D vector of CVs by year, time_step, and age
 */
void AgeLength::PopulateCV() {
  LOG_FINE() << "PopulateCV()";

  unsigned age     = min_age_;

  vector<string> time_steps = model_->time_steps();

  for (unsigned year_ndx = 0; year_ndx < cvs_.size(); ++year_ndx) {
    for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter) {
      if (!parameters_.Get(PARAM_CV_LAST)->has_been_defined()) {
        // TODO: Fix this #this test is robust but not compatible with testing framework, blah
        // If cv_last_ is not defined in the input then assume cv_first_ represents the cv for all age classes i.e constant cv
        LOG_FINEST() << "No cv_last defined";
        for (unsigned age_ndx = 0; age_ndx < model_->age_spread(); ++age_ndx) cvs_[year_ndx][step_iter][age_ndx] = (cv_first_);
      } else if (by_length_) {  // if passed the first test we have a min and max CV. So ask if this is linear interpolated by length at age
        LOG_FINEST() << "cv_last defined with by_length = true";
        age = min_age_;  // needs resetting for each year and timestep
        for (unsigned age_ndx = 0; age_ndx < model_->age_spread(); ++age_ndx, ++age) {
          cvs_[year_ndx][step_iter][age_ndx]
              = ((this->calculate_mean_length(model_years_[year_ndx], step_iter, age) - this->calculate_mean_length(model_years_[year_ndx], step_iter, min_age_))
                     * (cv_last_ - cv_first_)
                     / (this->calculate_mean_length(model_years_[year_ndx], step_iter, max_age_) - this->calculate_mean_length(model_years_[year_ndx], step_iter, min_age_))
                 + cv_first_);
        }
      } else {
        // else Do linear interpolation between cv_first_ and cv_last_ based on age class
        LOG_FINEST() << "cv_last defined with by_length = false";
        age = min_age_;  // needs resetting for each year and timestep
        for (unsigned age_ndx = 0; age_ndx < model_->age_spread(); ++age_ndx, ++age) {
          cvs_[year_ndx][step_iter][age_ndx] = (cv_first_ + (cv_last_ - cv_first_) * (age - min_age_) / (max_age_ - min_age_));
        }
      }  // if (!parameters_.Get(PARAM_CV_LAST)->has_been_defined()) {
    }    // for (unsigned step_iter = 0; step_iter < time_steps.size(); ++step_iter)
  }      // for (unsigned year : years) {
}

/**
 * Reset the age length class
 * This is called when the partition is reset
 */
void AgeLength::Reset() {
  LOG_FINE() << "Resetting age-length";
  DoReset();
  if (is_estimated_) {
    // Something is estimated in this class, so we should update everything.
    LOG_FINEST() << "Rebuilding cv lookup table";
    PopulateCV();
    //
    UpdateYearContainers();
    // Build Age-length matrix
    PopulateAgeLengthMatrix();
  } else {
    UpdateYearContainers();
  }
}
/**
 * this class is responsible for populating mean_length_by_timestep_age_ mean_weight_by_timestep_age_
 */

void AgeLength::UpdateYearContainers() {
  LOG_FINE() << "UpdateYearContainers " << model_->current_year();
  unsigned time_step_count = model_->time_steps().size();
  unsigned age             = min_age_;
  for (unsigned step_iter = 0; step_iter < time_step_count; ++step_iter) {
    age = min_age_;
    for (unsigned age_iter = 0; age_iter < model_->age_spread(); ++age_iter, ++age) {
      mean_length_by_timestep_age_[step_iter][age_iter] = calculate_mean_length(model_->current_year(), step_iter, age);
      mean_weight_by_timestep_age_[step_iter][age_iter]
          = length_weight_->mean_weight(mean_length_by_timestep_age_[step_iter][age_iter], distribution_, cvs_[model_->current_year() - year_offset_][step_iter][age_iter]);
    }
  }
}
/**
 * ReBuild Cache: called by the time_varying class
 */
void AgeLength::RebuildCache() {
  LOG_FINE() << "Rebuilding age-length cache for year " << model_->current_year() << " run mode " << model_->run_mode() << " state " << model_->state();
  // needs to go after Rebuild Cache
  PopulateCV();
  //
  UpdateYearContainers();
  // Build Age-length matrix
  UpdateAgeLengthMatrixForThisYear(model_->current_year());
}

/**
 * Return the units for the length-weight relationship units
 */
string AgeLength::weight_units() {
  return length_weight_->weight_units();
}

/**
 * Return mean weight for a given length, used by ProcessRemovalsByWeight
 */
Double AgeLength::mean_weight_by_length(Double length, unsigned age, unsigned year, unsigned time_step) {
 return length_weight_->mean_weight(length, distribution_, cvs_[year - year_offset_][time_step][age - age_offset_]);
}

/**
 * Processes and observations can call this to tell this agelength class to build an age-length class for these years.
 * Note the container which stores these years age_length_matrix_years_ will not be in sequential order.
 */
void AgeLength::BuildAgeLengthMatrixForTheseYears(vector<unsigned> years) {
  LOG_FINE() << "adding these years";
  if (age_length_matrix_years_.size() > 0) {
    unsigned add_this_year_counter = age_length_matrix_years_.size();
    for (unsigned i = 0; i < years.size(); ++i) {
      if (std::find(age_length_matrix_years_.begin(), age_length_matrix_years_.end(), years[i]) == age_length_matrix_years_.end()) {
        age_length_matrix_years_.push_back(years[i]);
        age_length_matrix_year_key_[years[i]] = add_this_year_counter;
        LOG_FINE() << "adding year " << years[i] << " with ndx = " << add_this_year_counter;

        ++add_this_year_counter;
      }
    }
  } else {
    for (unsigned i = 0; i < years.size(); ++i) {
      age_length_matrix_years_.push_back(years[i]);
      age_length_matrix_year_key_[years[i]] = i;
      LOG_FINE() << "adding year " << years[i] << " with ndx = " << i;

    }
  }
}

/*
* This will populate age_length_transition_matrix_ which has been allocated memory in the Build.
* This was taken from the previous code Partition::BuildAgeLengthProportions();
*/
void AgeLength::PopulateAgeLengthMatrix() {
  LOG_FINE() << "PopulateAgeLengthMatrix";
  // Some heavy lifting to be done here.
  unsigned year_count       = age_length_matrix_years_.size();
  unsigned time_step_count  = model_->time_steps().size();
  unsigned length_bin_count = model_->get_number_of_length_bins();
  unsigned year    = 0;
  year_dim_in_age_length_ = 0;
  Double   mu      = 0.0;
  Double   cv      = 0.0;
  unsigned age     = 0;
  Double   sigma   = 0.0;
  Double   Lvar    = 0.0;
  Double   tmp     = 0.0;
  Double   sum     = 0.0;
  vector<Double> cum(length_bin_count, 0.0);
  auto           model_length_bins = model_->length_bins();
  vector<double> length_bins(model_length_bins.size(), 0.0);
  LOG_FINE() << "years: " << year_count << "; time_steps: " << time_step_count << "; length_bins: " << length_bin_count;
  LOG_FINEST() << "length_bin_count: " << length_bin_count;
  if (distribution_ == Distribution::kLogNormal) {
    for (unsigned i = 0; i < model_length_bins.size(); ++i) {
      if (model_length_bins[i] < 0.0001)
        length_bins[i] = log(0.0001);
      else
        length_bins[i] = log(model_length_bins[i]);
    }
  } else {
    for (unsigned i = 0; i < model_length_bins.size(); ++i) 
      length_bins[i] = model_length_bins[i];
  }
  for(unsigned year_iter = 0; year_iter < age_length_matrix_years_.size(); ++year_iter) {
    year = age_length_matrix_years_[year_iter];
    year_dim_in_age_length_ = age_length_matrix_year_key_[age_length_matrix_years_[year_iter]];
    for (unsigned time_step = 0; time_step < time_step_count; ++time_step) {
      age = min_age_;
      for (unsigned age_index = 0; age_index < model_->age_spread(); ++age_index, ++age) {
        mu    = calculate_mean_length(year, time_step, age);
        cv    = cvs_[year - year_offset_][time_step - time_step_offset_][age - age_offset_];
        sigma = cv * mu;
      
        LOG_FINEST() << "year: " << year << "; age: " << age << "; mu: " << mu << "; cv: " << cv << "; sigma: " << sigma;
        if (distribution_ == Distribution::kLogNormal) {
          // Transform parameters in to log space
          Lvar    = log(cv * cv + 1.0);
          mu      = log(mu) - Lvar / 2.0;
          sigma   = sqrt(Lvar);
        }
        LOG_FINEST() << "year: " << year << "; age: " << age << "; mu: " << mu << "; cv: " << cv << "; sigma: " << sigma;

        //for (auto value : length_bins) LOG_FINEST() << "length_bin: " << value;

        LOG_FINEST() << "mu: " << mu;
        LOG_FINEST() << "sigma: " << sigma;
        sum                            = 0;
        vector<Double>& prop_in_length = age_length_transition_matrix_[year_dim_in_age_length_][time_step][age_index];
        
        for (unsigned j = 0; j < length_bin_count; ++j) {
          LOG_FINEST() << "calculating pnorm for length " << length_bins[j];
          // If we are using CASAL's Normal CDF function use this switch
          if (compatibility_ == PARAM_CASAL) {
            tmp = utilities::math::pnorm(length_bins[j], mu, sigma);
            LOG_FINE() << "casal_normal_cdf: " << tmp << " utilities::math::pnorm(" << length_bins[j] << ", " << mu << ", " << sigma;
          } else if (compatibility_ == PARAM_CASAL2) {
            tmp = utilities::math::pnorm2(length_bins[j], mu, sigma);
            LOG_FINE() << "normal: " << tmp << " utilities::math::pnorm(" << length_bins[j] << ", " << mu << ", " << sigma;
          } else {
            LOG_CODE_ERROR() << "Unknown compatibility option in the calculation of the distribution of age_length";
          }
          cum[j] = tmp;

          if (j > 0) {
            prop_in_length[j - 1] = cum[j] - cum[j - 1];
            sum += prop_in_length[j - 1];
            LOG_FINEST() << "prop_in_length[j - 1]: " << prop_in_length[j - 1] << ": " << cum[j] << ": " << cum[j - 1];
          }
        }  // for (unsigned j = 0; j < length_bin_count; ++j)

        if (model_->length_plus()) {
          prop_in_length[length_bin_count - 1] = 1.0 - sum - cum[0];
          LOG_FINEST() << "prop_in_length[length_bin_count - 1]: " << prop_in_length[length_bin_count - 1];
        } else {
          if (compatibility_ == PARAM_CASAL) {
            tmp = utilities::math::pnorm(length_bins[length_bin_count], mu, sigma);
            LOG_FINE() << "casal_normal_cdf: " << tmp << " utilities::math::pnorm(" << length_bins[length_bin_count] << ", " << mu << ", " << sigma;
          } else if (compatibility_ == PARAM_CASAL2) {
            tmp = utilities::math::pnorm2(length_bins[length_bin_count], mu, sigma);
            LOG_FINE() << "normal: " << tmp << " utilities::math::pnorm(" << length_bins[length_bin_count] << ", " << mu << ", " << sigma;
          } else {
            LOG_CODE_ERROR() << "Unknown compatibility option in the calculation of the distribution of age_length";
          }
          prop_in_length[length_bin_count - 1] = tmp -  cum[length_bin_count - 1];
        }
      }  // for (unsigned age_index = 0; age_index < iter.second->age_spread(); ++age_index)
    }    // for (unsigned time_step = 0; time_step < time_step_count; ++time_step)

    // If the age length object is not data, then it doesn't vary by year
    if (this->type() != PARAM_DATA) {
      auto& source = age_length_transition_matrix_[0];

      if (year_count > 1)
        for (unsigned year_idx = 1; year_idx < year_count; ++year_idx) {
          auto& props = age_length_transition_matrix_[year_idx];
          props       = source;
        }
      break;
    }
  }  // for (unsigned year_iter = 0; year_iter < year_count; ++year_iter)
}

/*
* This will populate age_length_transition_matrix_ for a specific year, called in Rebuild cache
* will first check if this is neccessary
*/
void AgeLength::UpdateAgeLengthMatrixForThisYear(unsigned year) {
  LOG_FINE() << "UpdateAgeLengthMatrixForThisYear";
  if (std::find(age_length_matrix_years_.begin(), age_length_matrix_years_.end(), year) != age_length_matrix_years_.end()) {
    unsigned time_step_count  = model_->time_steps().size();
    unsigned length_bin_count = model_->get_number_of_length_bins();
    year_dim_in_age_length_ = 0;
    Double   mu      = 0.0;
    Double   cv      = 0.0;
    unsigned age     = 0;
    Double   sigma   = 0.0;
    Double   Lvar    = 0.0;
    Double   tmp     = 0.0;
    Double   sum     = 0.0;
    vector<Double> cum(length_bin_count, 0.0);
    auto           model_length_bins = model_->length_bins();
    vector<double> length_bins(model_length_bins.size(), 0.0);
    LOG_FINEST() << "length_bin_count: " << length_bin_count;
    if (distribution_ == Distribution::kLogNormal) {
      for (unsigned i = 0; i < model_length_bins.size(); ++i) {
        if (model_length_bins[i] < 0.0001)
          length_bins[i] = log(0.0001);
        else
          length_bins[i] = log(model_length_bins[i]);
      }
    } else {
      for (unsigned i = 0; i < model_length_bins.size(); ++i) 
        length_bins[i] = model_length_bins[i];
    }

    
    year_dim_in_age_length_ = age_length_matrix_year_key_[year];
    for (unsigned time_step = 0; time_step < time_step_count; ++time_step) {
      age = min_age_;
      for (unsigned age_index = 0; age_index < model_->age_spread(); ++age_index, ++age) {
        mu    = calculate_mean_length(year, time_step, age);
        cv    = cvs_[year - year_offset_][time_step - time_step_offset_][age - age_offset_];
        sigma = cv * mu;
      
        LOG_FINEST() << "year: " << year << "; age: " << age << "; mu: " << mu << "; cv: " << cv << "; sigma: " << sigma;
        if (distribution_ == Distribution::kLogNormal) {
          // Transform parameters in to log space
          Lvar    = log(cv * cv + 1.0);
          mu      = log(mu) - Lvar / 2.0;
          sigma   = sqrt(Lvar);
        }
        LOG_FINEST() << "year: " << year << "; age: " << age << "; mu: " << mu << "; cv: " << cv << "; sigma: " << sigma;

        for (auto value : length_bins) LOG_FINEST() << "length_bin: " << value;

        LOG_FINEST() << "mu: " << mu;
        LOG_FINEST() << "sigma: " << sigma;

        sum                            = 0;
        vector<Double>& prop_in_length = age_length_transition_matrix_[year_dim_in_age_length_][time_step][age_index];
        for (unsigned j = 0; j < length_bin_count; ++j) {
          LOG_FINEST() << "calculating pnorm for length " << length_bins[j];
          // If we are using CASAL's Normal CDF function use this switch
          if (compatibility_ == PARAM_CASAL) {
            tmp = utilities::math::pnorm(length_bins[j], mu, sigma);
            LOG_FINE() << "casal_normal_cdf: " << tmp << " utilities::math::pnorm(" << length_bins[j] << ", " << mu << ", " << sigma;
          } else if (compatibility_ == PARAM_CASAL2) {
            tmp = utilities::math::pnorm2(length_bins[j], mu, sigma);
            LOG_FINE() << "normal: " << tmp << " utilities::math::pnorm(" << length_bins[j] << ", " << mu << ", " << sigma;
          } else {
            LOG_CODE_ERROR() << "Unknown compatibility option in the calculation of the distribution of age_length";
          }
          cum[j] = tmp;

          if (j > 0) {
            prop_in_length[j - 1] = cum[j] - cum[j - 1];
            sum += prop_in_length[j - 1];
            LOG_FINEST() << "prop_in_length[j - 1]: " << prop_in_length[j - 1] << ": " << cum[j] << ": " << cum[j - 1];
          }
        }  // for (unsigned j = 0; j < length_bin_count; ++j)

        if (model_->length_plus()) {
          prop_in_length[length_bin_count - 1] = 1.0 - sum - cum[0];
          LOG_FINEST() << "prop_in_length[length_bin_count - 1]: " << prop_in_length[length_bin_count - 1];
        }  else {
          if (compatibility_ == PARAM_CASAL) {
            tmp = utilities::math::pnorm(length_bins[length_bin_count], mu, sigma);
            LOG_FINE() << "casal_normal_cdf: " << tmp << " utilities::math::pnorm(" << length_bins[length_bin_count] << ", " << mu << ", " << sigma;
          } else if (compatibility_ == PARAM_CASAL2) {
            tmp = utilities::math::pnorm2(length_bins[length_bin_count], mu, sigma);
            LOG_FINE() << "normal: " << tmp << " utilities::math::pnorm(" << length_bins[length_bin_count] << ", " << mu << ", " << sigma;
          } else {
            LOG_CODE_ERROR() << "Unknown compatibility option in the calculation of the distribution of age_length";
          }
          prop_in_length[length_bin_count - 1] = tmp -  cum[length_bin_count - 1];
        }
      }  // for (unsigned age_index = 0; age_index < iter.second->age_spread(); ++age_index)
    }    // for (unsigned time_step = 0; time_step < time_step_count; ++time_step)
  } // if we need to update
}
/**
 * @details This will take numbers at age and pass them through the age-length transition matrix whilst applying a selectivity
 * it is assumed that this only happens in execute() so this method will have access to time-step and year.
 * @param numbers_at_age vector of numbers at age
 * @param numbers_at_length vector of numbers at length which has pre allocated memory and contains zeros
 * @param selectivity to apply for the age dimension.
 */
void AgeLength::populate_numbers_at_length(vector<Double> numbers_at_age, vector<Double>& numbers_at_length, Selectivity* selectivity){
  LOG_FINEST() << "populate_numbers_at_length";
  this_year_ = model_->current_year();
  this_time_step_ = model_->managers()->time_step()->current_time_step();
  year_dim_in_age_length_ = age_length_matrix_year_key_[this_year_];


  vector<double> length_bins            = model_->length_bins();
  unsigned size = model_->get_number_of_length_bins();

  LOG_FINE() << "Populating the age-length matrix for agelength class " << label_ << " in year " << this_year_ << " and time-step " << this_time_step_;
  LOG_FINE() << "Calculating age length data";
  for (unsigned age = min_age_; age <= max_age_; ++age) {
    unsigned i = age - min_age_;
    for (unsigned bin = 0; bin < size; ++bin) {
      numbers_by_age_length_transition_[i][bin] = selectivity->GetAgeResult(age, this) * numbers_at_age[i] * age_length_transition_matrix_[year_dim_in_age_length_][this_time_step_][i][bin];
      numbers_at_length[bin] += numbers_by_age_length_transition_[i][bin];
    }
  }
  // 

}

/**
 * @details This will take numbers at age and pass them through the age-length transition matrix 
 * it is assumed that this only happens in execute() so this method will have access to time-step and year.
 * @param numbers_at_age vector of numbers at age
 * @param numbers_at_length vector of numbers at length which has pre allocated memory and contains zeros
 */
void AgeLength::populate_numbers_at_length(vector<Double> numbers_at_age, vector<Double>& numbers_at_length){
  LOG_FINEST() << "populate_numbers_at_length";
  this_year_ = model_->current_year();
  this_time_step_ = model_->managers()->time_step()->current_time_step();
  year_dim_in_age_length_ = age_length_matrix_year_key_[this_year_];


  vector<double> length_bins            = model_->length_bins();
  unsigned size = model_->get_number_of_length_bins();
  LOG_FINE() << "Populating the age-length matrix for agelength class " << label_ << " in year " << this_year_ << " and time-step " << this_time_step_ << " year ndx = " << year_dim_in_age_length_;
  LOG_FINE() << "Calculating age length data";
  LOG_FINE() << "years in " << age_length_transition_matrix_.size();
  LOG_FINE() << "time_steps in " << age_length_transition_matrix_.size();

  for (unsigned age = min_age_; age <= max_age_; ++age) {
    unsigned i = age - min_age_;
    LOG_FINEST() << " age = " << age << " i = " << i;

    for (unsigned bin = 0; bin < size; ++bin) {
      LOG_FINEST() << " bin = " << bin;
      LOG_FINEST() << " prop = " << age_length_transition_matrix_[year_dim_in_age_length_][this_time_step_][i][bin];

      numbers_by_age_length_transition_[i][bin] = numbers_at_age[i] * age_length_transition_matrix_[year_dim_in_age_length_][this_time_step_][i][bin];
      numbers_at_length[bin] += numbers_by_age_length_transition_[i][bin];
    }
  }
  // 
}

/**
 * @details This will take numbers at age and pass them through the age-length transition matrix whilst applying a selectivity
 * it is assumed that this only happens in execute() so this method will have access to time-step and year.
 * @param numbers_at_age vector of numbers at age
 * @param numbers_at_length vector of numbers at length which has pre allocated memory and contains zeros
 * @param selectivity to apply for the age dimension.
 * @param map_length_bin_ndx vector col elements to amalgamate for the bespoke length bins, created by Model::get_map_for_bespoke_length_bins_to_global_length_bins()
 */
void AgeLength::populate_numbers_at_length(vector<Double> numbers_at_age, vector<Double>& numbers_at_length, Selectivity* selectivity, vector<int>& map_length_bin_ndx){
  LOG_FINEST() << "populate_numbers_at_length";
  this_year_ = model_->current_year();
  this_time_step_ = model_->managers()->time_step()->current_time_step();
  year_dim_in_age_length_ = age_length_matrix_year_key_[this_year_];
  unsigned size = model_->get_number_of_length_bins();

  LOG_FINE() << "Populating the age-length matrix for agelength class " << label_ << " in year " << this_year_ << " and time-step " << this_time_step_;
  LOG_FINE() << "Calculating age length data";
  for (unsigned age = min_age_; age <= max_age_; ++age) {
    unsigned i = age - min_age_;
    for (unsigned bin = 0; bin < size; ++bin) {
      if(map_length_bin_ndx[bin] >= 0) {   // values = -999 indicate see the function which makes this in the fucntion description
        numbers_at_length[map_length_bin_ndx[bin]] += selectivity->GetAgeResult(age, this) * numbers_at_age[i] * age_length_transition_matrix_[year_dim_in_age_length_][this_time_step_][i][bin];
      }
    }
  }
  // 

}

/**
 * @details This will take numbers at age and pass them through the age-length transition matrix 
 * it is assumed that this only happens in execute() so this method will have access to time-step and year.
 * @param numbers_at_age vector of numbers at age
 * @param numbers_at_length vector of numbers at length which has pre allocated memory and contains zeros
 * @param map_length_bin_ndx vector col elements to amalgamate for the bespoke length bins, created by Model::get_map_for_bespoke_length_bins_to_global_length_bins()
 */
void AgeLength::populate_numbers_at_length(vector<Double> numbers_at_age, vector<Double>& numbers_at_length, vector<int>& map_length_bin_ndx){
  LOG_FINEST() << "populate_numbers_at_length";
  this_year_ = model_->current_year();
  this_time_step_ = model_->managers()->time_step()->current_time_step();
  year_dim_in_age_length_ = age_length_matrix_year_key_[this_year_];


  vector<double> length_bins            = model_->length_bins();
  unsigned size = model_->get_number_of_length_bins();
  LOG_FINE() << "Populating the age-length matrix for agelength class " << label_ << " in year " << this_year_ << " and time-step " << this_time_step_ << " year ndx = " << year_dim_in_age_length_;
  LOG_FINE() << "Calculating age length data";
  LOG_FINE() << "years in " << age_length_transition_matrix_.size();
  LOG_FINE() << "time_steps in " << age_length_transition_matrix_.size();
  for (unsigned age = min_age_; age <= max_age_; ++age) {
    unsigned i = age - min_age_;
    LOG_FINEST() << " age = " << age << " i = " << i;

    for (unsigned bin = 0; bin < size; ++bin) {
      if(map_length_bin_ndx[bin] >= 0) {   // values = -999 indicate see the function which makes this in the fucntion description
        numbers_at_length[map_length_bin_ndx[bin]] += numbers_at_age[i] * age_length_transition_matrix_[year_dim_in_age_length_][this_time_step_][i][bin];
      }
    }
  }
}


/**
 * @details This will take the numbers at age and pass them through the age-length transition matrix for a specific length bin
 * which must be consistent with model length bins and store in an age based vector. Currently only the process TagByLength I think uses this.
 * it is assumed that this only happens in execute() so this method will have access to time-step and year.
 * @param numbers_at_age vector of numbers at age to go through the age-length-transition matrix
 * @param numbers_at_age_with_exploitation vector of numbers at age to store values in
 * @param exploitation_by_length exploitation value by length
 * @param model_length_bin_ndx an index for the length bin.
 */
void  AgeLength::populate_numbers_at_age_with_length_based_exploitation(vector<Double>& numbers_at_age, vector<Double>& numbers_at_age_with_exploitation, Double& exploitation_by_length, unsigned model_length_bin_ndx, Selectivity* selectivity) {
  LOG_FINEST() << "populate_numbers_at_age_with_length_based_exploitation";
  this_year_ = model_->current_year();
  this_time_step_ = model_->managers()->time_step()->current_time_step();
  year_dim_in_age_length_ = age_length_matrix_year_key_[this_year_];
  if(model_length_bin_ndx >= model_->get_number_of_length_bins()) 
    LOG_CODE_ERROR() << "this function can only deal with length bins that are consistent with the model length bins. this function recieved a length bin = " << model_length_bin_ndx << " but there are only " << model_->get_number_of_length_bins() << " model length bins";
  
  LOG_FINE() << "Populating the age-length matrix for agelength class " << label_ << " in year " << this_year_ << " and time-step " << this_time_step_ << " year ndx = " << year_dim_in_age_length_;
  for (unsigned age = min_age_; age <= max_age_; ++age) {
    unsigned i = age - min_age_;
    //LOG_FINEST() << " age = " << age << " i = " << i;
    numbers_at_age_with_exploitation[i] += numbers_at_age[i] * age_length_transition_matrix_[year_dim_in_age_length_][this_time_step_][i][model_length_bin_ndx] * exploitation_by_length * selectivity->GetAgeResult(age, this);
  }
  // 

}

/**
 * @details FillReportCache
 * populates neccessary information that we want to report for this current year.
 */
void AgeLength::FillReportCache(ostringstream& cache) {
  unsigned min_age    = model_->min_age();
  unsigned max_age    = model_->max_age();
  unsigned year       = model_->current_year();
  unsigned this_time_step = model_->managers()->time_step()->current_time_step();
  unsigned year_dim_in_age_length = age_length_matrix_year_key_[year];
  LOG_FINE() << "FillReportCache time step = " << this_time_step << " year = " << year;

  cache << "cvs_by_age: ";
  for (unsigned age = min_age; age <= max_age; ++age) {
    cache << cvs_[year - year_offset_][this_time_step - time_step_offset_][age - age_offset_] << " ";
  }
  cache << REPORT_EOL;

  cache << "mean_length_at_age: ";
  for (unsigned age = min_age; age <= max_age; ++age) {
    cache << mean_length_by_timestep_age_[this_time_step - time_step_offset_][age - age_offset_] << " ";
  }
  cache << REPORT_EOL;
  cache << "mean_weight_at_age: ";
  for (unsigned age = min_age; age <= max_age; ++age) {
    cache << mean_weight_by_timestep_age_[this_time_step - time_step_offset_][age - age_offset_] << " ";
  }
  cache << REPORT_EOL;

  if (std::find(age_length_matrix_years_.begin(), age_length_matrix_years_.end(), year) != age_length_matrix_years_.end()) {
    LOG_FINE() << "printing Age length matrix";

    // print age-length matrix for this year
    cache << "age_length_transition_matrix " << REPORT_R_DATAFRAME << REPORT_EOL;
    cache << "year time_step age ";
    for (auto length : model_->length_bins()) 
      cache << "L(" << length << ") ";
    cache << REPORT_EOL;
    for (unsigned age = min_age; age <= max_age; ++age) {
      unsigned i = age - min_age;
      cache << year << " " << this_time_step << " " << age << " ";
      for (unsigned k = 0; k < age_length_transition_matrix_[year_dim_in_age_length][this_time_step][i].size(); ++k) {
        cache << age_length_transition_matrix_[year_dim_in_age_length][this_time_step][i][k] << " ";
      }
      cache << REPORT_EOL;
    }
  }
}




} /* namespace niwa */
