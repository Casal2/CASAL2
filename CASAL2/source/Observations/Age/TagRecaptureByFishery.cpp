/**
 * @file TagRecaptureByFisheryAndFishery.cpp
 * @author C.Marsh
 * @github https://github.com/Craig44
 * @date 2023
 * @section LICENSE
 *
 * Copyright NIWA Science 2023 - www.niwa.co.nz
 *
 */

// headers
#include "TagRecaptureByFishery.h"

#include <algorithm>

#include "Categories/Categories.h"
#include "Model/Model.h"
#include "Partition/Accessors/All.h"
#include "Selectivities/Manager.h"
#include "TimeSteps/Manager.h"
#include "Utilities/Map.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"
#include "Utilities/Vector.h"
#include "Processes/Manager.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
// namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 * Default constructor
 */
TagRecaptureByFishery::TagRecaptureByFishery(shared_ptr<Model> model) : Observation(model) {
  recaptures_table_ = new parameters::Table(PARAM_RECAPTURED);

  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for which there are observations", "");

  parameters_.Bind<string>(PARAM_TAGGED_CATEGORIES, &tagged_category_labels_,"The categories of tagged individuals for the observation", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The label of the time step that the observation occurs in", "");

  parameters_.Bind<string>(PARAM_METHOD_OF_REMOVAL, &method_, "The label of the observed method of removals", "", "");
  parameters_.BindTable(PARAM_RECAPTURED, recaptures_table_, "The table of observed recaptured individuals in each age class", "", false);
  parameters_.Bind<string>(PARAM_MORTALITY_PROCESS, &process_label_, "The label of the mortality process for the observation", "");
  parameters_.Bind<Double>(PARAM_REPORTING_RATE, &reporting_rate_, "The probability of detecting a recaptured individual", "")->set_range(0.0, 1.0);

  RegisterAsAddressable(PARAM_REPORTING_RATE, &reporting_rate_);

  allowed_likelihood_types_.push_back(PARAM_POISSON);
  //allowed_likelihood_types_.push_back(PARAM_NEGATIVE_BINOMIAL);

  allowed_mortality_types_.push_back(PARAM_MORTALITY_INSTANTANEOUS);
  allowed_mortality_types_.push_back(PARAM_MORTALITY_HYBRID);
}

/**
 * Validate configuration file parameters
 */
void TagRecaptureByFishery::DoValidate() {

  for (auto year : years_) {
    if ((year < model_->start_year()) || (year > model_->final_year()))
      LOG_ERROR_P(PARAM_YEARS) << "Years cannot be less than start_year (" << model_->start_year() << "), or greater than final_year (" << model_->final_year() << ").";
  }


  if (delta_ < 0.0)
    LOG_ERROR_P(PARAM_DELTA) << ": delta (" << delta_ << ") cannot be less than 0.0";

  /**
   * Validate the number of obs provided matches age spread * tagged_category_labels_ * years
   * This is because we'll have 1 set of obs per category collection provided.
   * categories male+female male = 2 collections
   */
  unsigned                obs_expected = 2;
  vector<vector<string>>& obs_data     = recaptures_table_->data();
  if (obs_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_RECAPTURED) << " has " << obs_data.size() << " rows defined, but " << years_.size() << " should match the number of years provided";
  }

  for (vector<string>& obs_data_line : obs_data) {
    if (obs_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_RECAPTURED) << " has " << obs_data_line.size() << " values defined, but " << obs_expected << " should match 2 one year and the other for recaptures among all tagged categories";
    }

    unsigned year = 0;
    if (!utilities::To<unsigned>(obs_data_line[0], year))
      LOG_ERROR_P(PARAM_RECAPTURED) << " value " << obs_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_ERROR_P(PARAM_RECAPTURED) << " value " << year << " is not a valid year for this observation";

    for (unsigned i = 1; i < obs_data_line.size(); ++i) {
      double value = 0.0;
      if (!utilities::To<double>(obs_data_line[i], value))
        LOG_ERROR_P(PARAM_RECAPTURED) << " value (" << obs_data_line[i] << ") could not be converted to a Double";
      observed_recaptures_[year].push_back(value);
    }
    expected_recaptures_[year].resize(1, 0.0);
    if (observed_recaptures_[year].size() != obs_expected - 1)
      LOG_FATAL_P(PARAM_RECAPTURED) << " " << observed_recaptures_[year].size() << " ages were supplied, but " << obs_expected - 1 << " ages are required";
  }

  if (time_step_label_.size() != method_.size()) {
    LOG_ERROR_P(PARAM_TIME_STEP) << "Specify the same number of time step labels as methods. " << time_step_label_.size() << " time-step labels were specified, but "
                                 << method_.size() << " methods were specified";
  }
}

/**
 * Build any runtime relationships and ensure that the labels for other objects are valid.
 */
void TagRecaptureByFishery::DoBuild() {
  partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, tagged_category_labels_));

 for (string time_label : time_step_label_) {
   auto time_step = model_->managers()->time_step()->GetTimeStep(time_label);
   if (!time_step) {
     LOG_FATAL_P(PARAM_TIME_STEP) << "Time step label " << time_label << " was not found.";
   } else {
     auto process       = time_step->SubscribeToProcess(this, years_, process_label_);
     if(!process)
       LOG_FATAL_P(PARAM_MORTALITY_PROCESS) << "could not find process " << process_label_;
     mortality_process_ = model_->managers()->process()->GetAgeBasedMortalityProcess(process_label_);
   }
 }

   if (mortality_process_ == nullptr) {
     LOG_FATAL_P(PARAM_MORTALITY_PROCESS) << "Could not find process " << process_label_ << ".";
   } else {
     if(find(allowed_mortality_types_.begin(), allowed_mortality_types_.end(), mortality_process_->type()) == allowed_mortality_types_.end())
       LOG_FATAL_P(PARAM_MORTALITY_PROCESS) << "The mortality process is of type " << mortality_process_->type() << " is not allowed for this observation.";
   }
   // Need to split the categories if any are combined for checking
   vector<string> temp_split_tagged_category_labels, split_tagged_category_labels;

   for (const string& category_label : tagged_category_labels_) {
     boost::split(temp_split_tagged_category_labels, category_label, boost::is_any_of("+"));
     for (const string& split_category_label : temp_split_tagged_category_labels) {
       split_tagged_category_labels.push_back(split_category_label);
     }
   }
   for (auto category : split_tagged_category_labels)
     LOG_FINEST() << category;

   if(split_tagged_category_labels.size() != tagged_category_labels_.size())
     LOG_ERROR_P(PARAM_TAGGED_CATEGORIES) << "You cannot specify grouped categories using the '+' syntax. Please specify tagged categoreis using a space seperator.";
   // Do some checks so that the observation and process are compatible
   if (!mortality_process_->check_methods_for_removal_obs(method_))
     LOG_FATAL_P(PARAM_METHOD_OF_REMOVAL) << "could not find all these methods in the instantaneous_mortality process labeled " << process_label_
                                          << ". Check that the methods are compatible with this process";
   if (!mortality_process_->check_categories_in_methods_for_removal_obs(method_, tagged_category_labels_))
     LOG_FATAL_P(PARAM_TAGGED_CATEGORIES) << "could not find all these categories in the instantaneous_mortality process labeled " << process_label_
                                   << ". Check that the categories are compatible with this process";
   if (!mortality_process_->check_years_in_methods_for_removal_obs(years_, method_))
     LOG_FATAL_P(PARAM_YEARS) << "could not find catches with catch in all years in the instantaneous_mortality process labeled " << process_label_
                              << ". Check that the years are compatible with this process";
   fishery_ndx_to_get_catch_at_info_ = mortality_process_->get_fishery_ndx_for_catch_at(method_);
   LOG_FINE() << "fishery ndx = ";
   for(auto fishndx : fishery_ndx_to_get_catch_at_info_)
     LOG_FINE() << fishndx;
   vector<unsigned> category_ndxs = mortality_process_->get_category_ndx_for_catch_at(tagged_category_labels_);
   for(unsigned category_ndx = 0; category_ndx < tagged_category_labels_.size(); ++category_ndx)  {
     category_lookup_for_ndx_to_get_catch_at_info_[tagged_category_labels_[category_ndx]] = category_ndxs[category_ndx];
     LOG_FINE() << "cat " << tagged_category_labels_[category_ndx] << " ndx = " << category_ndxs[category_ndx];
   }
   year_ndx_to_get_catch_at_info_ = mortality_process_->get_year_ndx_for_catch_at(years_);
   LOG_FINE() << "year ndx = ";
   for(auto yearndx : year_ndx_to_get_catch_at_info_)
     LOG_FINE() << yearndx;

   // If this observation is made up of multiple methods lets find out the last one, because that is when we execute the process
   vector<unsigned> time_step_index;
   for (string label : time_step_label_) time_step_index.push_back(model_->managers()->time_step()->GetTimeStepIndex(label));

   unsigned last_method_time_step = 9999;
   if (time_step_index.size() > 1) {
     for (unsigned i = 0; i < (time_step_index.size() - 1); ++i) {
       if (time_step_index[i] > time_step_index[i + 1])
         last_method_time_step = time_step_index[i];
       else
         last_method_time_step = time_step_index[i + 1];
     }
     time_step_to_execute_ = last_method_time_step;
   } else {
     time_step_to_execute_ = time_step_index[0];
   }
   LOG_FINEST() << "time step to execute tag-recapture observation in = " << time_step_to_execute_;

}

/**
 * This method is called at the start of the targeted
 * time step for this observation.
 *
 * Build the cache for the partition
 * structure to use with any interpolation
 */
void TagRecaptureByFishery::PreExecute() {

}

/**
 * Execute
 */
void TagRecaptureByFishery::Execute() {
  LOG_FINEST() << "Entering observation " << label_;
  unsigned year = model_->current_year();
  std::pair<bool, int >  this_year_iter = utilities::findInVector(years_, year);
  if(!this_year_iter.first)
    LOG_CODE_ERROR() << "if(!this_year_iter.first)";
  LOG_FINEST() << "current year " << year;
  // Check if we are in the final time_step so we have all the relevent information from the Mortaltiy process
  unsigned current_time_step = model_->managers()->time_step()->current_time_step();
  LOG_FINEST() << "current time-step = " << current_time_step;
  if (time_step_to_execute_ == current_time_step) {
    auto                                                     partition_iter  = partition_->Begin();  // vector<vector<partition::Category> >
    for (unsigned category_offset = 0; category_offset < tagged_category_labels_.size(); ++category_offset, ++partition_iter) {
      LOG_FINEST() << "Category = " << tagged_category_labels_[category_offset];
      auto category_iter = partition_iter->begin();
      for (; category_iter != partition_iter->end(); ++category_iter) {
        // Go through all the fisheries and accumulate the expectation whilst also applying ageing error
        unsigned method_offset = 0;
        for (string fishery : method_) {
          LOG_FINEST() << "year ndx = " << year_ndx_to_get_catch_at_info_[this_year_iter.second] << " fishery ndx = " << fishery_ndx_to_get_catch_at_info_[method_offset] << " category ndx = " << category_lookup_for_ndx_to_get_catch_at_info_[(*category_iter)->name_];
          vector<Double>& Removals_at_age = mortality_process_->get_catch_at_by_year_fishery_category(year_ndx_to_get_catch_at_info_[this_year_iter.second], fishery_ndx_to_get_catch_at_info_[method_offset], category_lookup_for_ndx_to_get_catch_at_info_[(*category_iter)->name_]);
          LOG_FINE() << "Nages = " << Removals_at_age.size();
          /*
           *  Now collapse the number_age into the expected_values for the observation
           */
          for (unsigned k = 0; k < Removals_at_age.size(); ++k) {
            LOG_FINE() << "----------";
            LOG_FINE() << "Fishery: " << fishery;
            LOG_FINE() << "Numbers At Age After Ageing error: " << (*category_iter)->min_age_ + k << " for category " << (*category_iter)->name_ << " "
                       << Removals_at_age[k];
            expected_recaptures_[year][0] += Removals_at_age[k] * reporting_rate_; // recoveries x reporting rate

          }

          if (expected_recaptures_[year].size() != observed_recaptures_[model_->current_year()].size())
            LOG_CODE_ERROR() << "expected_recaptures_[year].size(" << expected_recaptures_[year].size() << ") != observed_recaptures_[year].size("
                             << observed_recaptures_[model_->current_year()].size() << ")";


          method_offset++;
        } // for (string fishery : method_) {
      }  // for (unsigned k = 0; k < Removals_at_age.size(); ++k) {
    } //    for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {

    /**
     * save our comparisons so we can use them to generate the score from the likelihoods later
     */

    LOG_FINEST() << "-----";
    SaveComparison(label_, expected_recaptures_[year][0],
        observed_recaptures_[year][0], 0.0,
        AS_DOUBLE(expected_recaptures_[year][0]), 0.0, delta_, 0.0);

  } // if (time_step_to_execute_ == current_time_step) {
} // Execute()

/**
 * This method is called at the end of a model iteration
 * to calculate the score for the observation.
 */
void TagRecaptureByFishery::CalculateScore() {
  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
  */
  LOG_FINEST() << "Calculating neglogLikelihood for observation = " << label_;

  if (model_->run_mode() == RunMode::kSimulation) {
    likelihood_->SimulateObserved(comparisons_);
  } else {
    likelihood_->GetScores(comparisons_);

    for (unsigned year : years_) {
      scores_[year] = 0.0;
      LOG_FINEST() << "-- Observation neglogLikelihood calculation " << label_;
      LOG_FINEST() << "[" << year << "] Initial neglogLikelihood:" << scores_[year];
      for (obs::Comparison comparison : comparisons_[year]) {
        LOG_FINEST() << "[" << year << "] + neglogLikelihood: " << comparison.score_;
        scores_[year] += comparison.score_;
      }
    }

    LOG_FINEST() << "Finished calculating neglogLikelihood for = " << label_;
  }

}

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */
