/**
 * @file GrowthIncrement.cpp
 * @author  C.Marsh
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * For length based models - this class are responsible for describing movement of fish betwenn length bin partitions
 */

// headers
#include "GrowthIncrement.h"

#include "../TimeSteps/Manager.h"
#include "../Model/Model.h"
#include "../LengthWeights/LengthWeight.h"
#include "../LengthWeights/Manager.h"
// namespaces
namespace niwa {

/**
 * Default constructor
 */
GrowthIncrement::GrowthIncrement(shared_ptr<Model> model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label of the growth increment model", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of the growth increment model", "");
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_label_, "The assumed distribution for the growth model", "", PARAM_NORMAL)->set_allowed_values({PARAM_NORMAL});
  parameters_.Bind<string>(PARAM_LENGTH_WEIGHT, &length_weight_label_, "The label from an associated length-weight block", "");
  parameters_.Bind<Double>(PARAM_CV, &cv_, "Coeffecient of variaion for the growth increment model", "");
  parameters_.Bind<Double>(PARAM_MIN_SIGMA, &min_sigma_, "Lower bound on sigma for the growth mode", "");
  parameters_.Bind<string>(PARAM_COMPATIBILITY, &compatibility_,"Backwards compatibility option: either casal2 (the default) or casal to use the (less accurate) cumulative normal function from CASAL", "", PARAM_CASAL2)->set_allowed_values({PARAM_CASAL, PARAM_CASAL2});
  parameters_.Bind<Double>( PARAM_TIME_STEP_PROPORTIONS, &time_step_proportions_,"The fraction of the year applied in each time step that is added to the age for the purposes of evaluating the length, i.e., a value of 0.5 for a time step will evaluate the length of individuals at age+0.5 in that time step","", false)->set_range(0.0, 1.0);

  RegisterAsAddressable(PARAM_CV, &cv_);
  RegisterAsAddressable(PARAM_MIN_SIGMA, &min_sigma_);

}

/**
 * Validate the GrowthIncrement objects
 */
void GrowthIncrement::Validate() {
  LOG_TRACE();
  parameters_.Populate(model_);

  if (distribution_label_ == PARAM_NORMAL)
    distribution_ = Distribution::kNormal;
  else if (distribution_label_ == PARAM_LOGNORMAL)
    distribution_ = Distribution::kLogNormal;
  else
    LOG_ERROR() << "The age-length distribution '" << distribution_label_ << "' is not valid.";

  if(compatibility_ == PARAM_CASAL) {
    compatibility_type_ = CompatibilityType::kCasal;
  } else if(compatibility_ == PARAM_CASAL2) {
    compatibility_type_ = CompatibilityType::kCasal2;
  }

  number_of_model_length_bins_ = model_->get_number_of_length_bins();
  model_min_length_bins_ = model_->length_bins();
  plus_group_ = model_->length_plus();
  model_length_midpoints_ = model_->length_bin_mid_points();
  mean_weight_by_length_bin_index_.resize(number_of_model_length_bins_, 0.0);
  // calculate midpoints

  DoValidate();
}

/**
 * Build the GrowthIncrement objects
 */
void GrowthIncrement::Build() {
  LOG_TRACE();
  // allocate memory for growth transition matrix
  unsigned time_step_count = model_->time_steps().size();
  growth_transition_matrix_.resize(time_step_count);
  for(unsigned i = 0; i < time_step_count; ++i) {
    growth_transition_matrix_[i].resize(number_of_model_length_bins_);
    for(unsigned j = 0; j < number_of_model_length_bins_; ++j) 
      growth_transition_matrix_[i][j].resize(number_of_model_length_bins_, 0.0);
  }
  LOG_FINE() << "ALlocated memory for growth transition matrix";

  LOG_FINE() << "Get Length Weigth pointer";
  // Get length weight pointer
  length_weight_ = model_->managers()->length_weight()->GetLengthWeight(length_weight_label_);
  if (!length_weight_)
    LOG_ERROR_P(PARAM_LENGTH_WEIGHT) << "Length-weight label " << length_weight_label_ << " was not found";
  // Subscribe this class to this length_weight class
  // This creates a dependency, for the timevarying functionality. See NotifySubscribers() called in TimeVarying.cpp line 107:108
  length_weight_->SubscribeToRebuildCache(this);
  // if this length weight class time-varies then it will flag this age-length class to rebuild cache

  // check time-steps are correct
  Double total_time_step_prop = 0.0;
  for(auto time_value : time_step_proportions_)
    total_time_step_prop += time_value;
  if(!utilities::math::IsOne(total_time_step_prop))
      LOG_ERROR_P(PARAM_TIME_STEP_PROPORTIONS) << "The sum of these values needs to equal 1.0. The values supplied = " << total_time_step_prop;
  if (time_step_count != time_step_proportions_.size()) {
    LOG_ERROR_P(PARAM_TIME_STEP_PROPORTIONS) << "size (" << time_step_proportions_.size() << ") must match the number "
                                             << "of defined time steps for this Model (" << time_step_count << ")";
  }

  DoBuild(); 
  populate_growth_transition_matrix();
  LOG_FINE() << "calculate mean weight";
  for(unsigned j = 0; j < number_of_model_length_bins_; ++j) 
    mean_weight_by_length_bin_index_[j] = length_weight_->mean_weight(model_length_midpoints_[j]);

}
/**
 * Rebuild cache objects in the GrowthIncrement objects
 */
void GrowthIncrement::RebuildCache() {
  // if time-vary any of these parameters update parameters
  populate_growth_transition_matrix();
}

/**
 * Build the GrowthIncrement objects
 */
void GrowthIncrement::Reset() {
  DoReset();
}


/**
 * Method for populating growth transition matrix 
*/
void GrowthIncrement::populate_growth_transition_matrix() { 
  LOG_TRACE();
  unsigned time_step_count = model_->time_steps().size();
  Double sum_so_far = 0.0;
  Double mu = 0.0;
  Double sigma = 0.0;
  if (compatibility_type_ == CompatibilityType::kCasal) {
    for (unsigned step_iter = 0; step_iter < time_step_count; ++step_iter) {
      for(unsigned i = 0; i < number_of_model_length_bins_; ++i)  {
        if((i == (number_of_model_length_bins_ - 1)) & plus_group_) {
          LOG_FINE() << "in plus group set = 1.0";
          growth_transition_matrix_[step_iter][i][i] = 1.0; // stay in plus group
        } else {
          mu = get_mean_increment(model_length_midpoints_[i]);
          sigma = fmax(min_sigma_, mu * cv_);
          growth_transition_matrix_[step_iter][i][i] =  utilities::math::pnorm(model_min_length_bins_[i + 1] - model_length_midpoints_[i], mu, sigma);
          LOG_FINE() << " i = " << i + 1 << " mu = " << mu << " sigma = " << sigma << " val " << model_min_length_bins_[i + 1] - model_length_midpoints_[i] << " pnorm " << growth_transition_matrix_[step_iter][i][i];
          sum_so_far = growth_transition_matrix_[step_iter][i][i];
          for (unsigned j = i + 1; j < number_of_model_length_bins_; j++) {
            growth_transition_matrix_[step_iter][i][j] = utilities::math::pnorm(model_min_length_bins_[j + 1] - model_length_midpoints_[i], mu, sigma) - sum_so_far;
            sum_so_far += growth_transition_matrix_[step_iter][i][j];
            LOG_FINE() << "sum so far " << sum_so_far;
          }
          if (plus_group_){
            growth_transition_matrix_[step_iter][i][number_of_model_length_bins_ - 1] = 1.0 - sum_so_far;
          } else {
            LOG_FINEST() << "pnorm = " << utilities::math::pnorm(model_min_length_bins_[number_of_model_length_bins_] - model_length_midpoints_[i], mu, sigma) << " value = " << model_min_length_bins_[number_of_model_length_bins_] << " midpoint " << model_length_midpoints_[i];
            growth_transition_matrix_[step_iter][i][number_of_model_length_bins_ - 1] = utilities::math::pnorm(model_min_length_bins_[number_of_model_length_bins_] - model_length_midpoints_[i], mu, sigma) - sum_so_far;
          }
        }
      }
    }
  } else {
    for (unsigned step_iter = 0; step_iter < time_step_count; ++step_iter) {
      for(unsigned i = 0; i < number_of_model_length_bins_; ++i)  {
        if((i == (number_of_model_length_bins_ - 1)) & plus_group_) {
          growth_transition_matrix_[step_iter][i][i] = 1.0; // stay in plus group
        } else {
          mu = get_mean_increment(model_length_midpoints_[i]);
          sigma = fmax(min_sigma_, mu * cv_);
          LOG_FINE() << " i = " << i + 1 << " mu = " << mu << " sigma = " << sigma;
          growth_transition_matrix_[step_iter][i][i] =  utilities::math::pnorm2(model_min_length_bins_[i + 1] - model_length_midpoints_[i], mu, sigma);
          sum_so_far = growth_transition_matrix_[step_iter][i][i];
          for (unsigned j = i + 1; j < number_of_model_length_bins_; j++) {
            growth_transition_matrix_[step_iter][i][j] = utilities::math::pnorm2(model_min_length_bins_[j + 1] - model_length_midpoints_[i], mu, sigma) - sum_so_far;
            sum_so_far += growth_transition_matrix_[step_iter][i][j];
            LOG_FINE() << "sum so far " << sum_so_far;
          }
          if (plus_group_){
            growth_transition_matrix_[step_iter][i][number_of_model_length_bins_ - 1] = 1.0 - sum_so_far;
          } else {
            growth_transition_matrix_[step_iter][i][number_of_model_length_bins_ - 1] = utilities::math::pnorm2(model_min_length_bins_[number_of_model_length_bins_] - model_length_midpoints_[i], mu, sigma) - sum_so_far;
          }
        }
      }
    }
  }
}
/**
 * Save info for reporting. Note this is called for every year the user asks
 * 
*/
void GrowthIncrement::FillReportCache(ostringstream& cache) {
  LOG_TRACE();
  unsigned this_time_step         = model_->managers()->time_step()->current_time_step();
  cache << "distribution: " << distribution_label_<< REPORT_EOL;
  cache << "time_step_proportions: ";
  for(auto time_step_val :time_step_proportions_)
    cache << time_step_val << " ";
  cache << REPORT_EOL;
  cache << "min_sigma: " << min_sigma_ << REPORT_EOL;
  cache << "cv: " << cv_<< REPORT_EOL;

  cache << "values " << REPORT_R_DATAFRAME << REPORT_EOL;
  cache << "length_bin_index length_midpoint mean sigma mean_weight" << REPORT_EOL;
  Double mu = 0.0;
  Double sigma = 0.0;
  for(unsigned i = 0; i < number_of_model_length_bins_; ++i)  {
    mu = get_mean_increment(model_length_midpoints_[i]);
    sigma = fmax(min_sigma_, mu * cv_);
    cache << i + 1 << " " << model_length_midpoints_[i] << " " << AS_DOUBLE(mu) << " " << AS_DOUBLE(sigma) << " " << mean_weight_by_length_bin_index_[i] << REPORT_EOL;
  }
  cache << "growth_transition_matrix_timestep_" << this_time_step + 1 << " " << REPORT_R_MATRIX << REPORT_EOL;
  for(unsigned i = 0; i < number_of_model_length_bins_; ++i)  {
    for(unsigned j = 0; j < number_of_model_length_bins_; ++j)  {
      cache << growth_transition_matrix_[this_time_step][i][j] << " ";
    }
    cache << REPORT_EOL;
  }
}



} /* namespace niwa */
