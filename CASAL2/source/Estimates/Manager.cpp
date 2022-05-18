/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 27/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Manager.h"

#include <algorithm>

#include "../GlobalConfiguration/GlobalConfiguration.h"
#include "../Model/Managers.h"
#include "../Model/Model.h"
#include "../Reports/Common/EstimateValue.h"
#include "../Reports/Common/MPD.h"
#include "../Reports/Manager.h"
#include "../Utilities/Math.h"

// Namespaces
namespace niwa {
namespace estimates {

using std::set;
namespace math = niwa::utilities::math;

/**
 * Destructor
 */
Manager::~Manager() {
  for (auto creator : creators_) delete creator;
}

/**
 * Validate the objects - no model
 */
void Manager::Validate() {
  LOG_CODE_ERROR() << "This method is not supported";
}

/**
 * Validate the objects
 */
void Manager::Validate(shared_ptr<Model> model) {
  LOG_TRACE();
  /**
   * Run over our creators and get them to build the actual
   * estimates the system is going to build.
   *
   * Firstly we need to isolate any estimates that will reference
   * another estimate as these will need to be created after the others
   */
  vector<Creator*> delayed_creators;
  for (Creator* creator : creators_) {
    creator->parameters().Populate(model);

    if (creator->parameter().substr(0, 8) == PARAM_ESTIMATE) {
      delayed_creators.push_back(creator);
      continue;
    }

    creator->CreateEstimates();
  }

  // nothing fancy here, we don't support chain estimates past 1 level
  LOG_FINE() << delayed_creators.size() << " estimates were delayed because they reference other estimates";
  for (auto creators : delayed_creators) {
    creators->CreateEstimates();
  }

  /**
   * Validate the actual estimates now
   */
  for (auto estimate : objects_) estimate->Validate();

  /**
   * Remove any estimates where the bounds are the same.
   */
  unsigned count = objects_.size();
  objects_.erase(std::remove_if(objects_.begin(), objects_.end(), [](Estimate* estimate) { return math::IsEqual(estimate->lower_bound(), estimate->upper_bound()); }),
                 objects_.end());
  if (count != objects_.size())
    LOG_WARNING() << "Estimates were removed because of matching lower and upper bounds. Originally had " << count << " estimates, now have " << objects_.size();
}

/**
 * Build the objects - no model
 */
void Manager::Build() {
  LOG_CODE_ERROR() << "This method is not supported";

}

/**
 * Build the objects
 */
void Manager::Build(shared_ptr<Model> model) {
  LOG_TRACE();
  for (auto estimate : objects_) {
    estimate->Build();
  }
  // Check there are no repeat parameters in @estimates or same
  vector<string> param_labels_to_check;
  vector<string> param_locations;
  for (auto estimate : objects_) {
    LOG_FINE() << "checking estimate with param " << estimate->parameter() ;
    // check parameter 
    auto iter = find(param_labels_to_check.begin(), param_labels_to_check.end(), estimate->parameter());
    if(iter !=  param_labels_to_check.end()) {
      LOG_ERROR() << estimate->location() << ": this @estimate has parameter " << estimate->parameter() << ". This was found in @estimate block at " << param_locations[distance(param_labels_to_check.begin(), iter)];
    }
    param_labels_to_check.push_back(estimate->parameter());
    param_locations.push_back(estimate->location());
    for(auto same_param : estimate->same_labels()) {
      LOG_FINE() << "checking same param " << same_param ;
      auto same_iter = find(param_labels_to_check.begin(), param_labels_to_check.end(), same_param);

      if(same_iter !=  param_labels_to_check.end()) {
        LOG_ERROR() << estimate->location() << ": this @estimate has 'same' parameter " << same_param << ". This was found in @estimate block at " << param_locations[distance(param_labels_to_check.begin(), same_iter)];
      }
      param_labels_to_check.push_back(same_param);
      param_locations.push_back(estimate->location());
    }
  }

  LOG_FINEST() << "Finished building all estimates";
  if (model->is_primary_thread_model() && model->global_configuration().create_mpd_output_file()) {
    string mpd_file_name  = model->global_configuration().get_mpd_output_file();
    string mpd_write_mode = model->global_configuration().mpd_write_mode();
    LOG_IMPORTANT() << "Setting '" << mpd_file_name << "' as the output file for the MPD (i.e., free parameters and covariance)";
    model->managers()->report()->Pause();
    reports::MPD* report = new reports::MPD();
    report->set_block_type(PARAM_REPORT);
    report->set_defined_file_name(__FILE__);
    report->set_defined_line_number(__LINE__);
    report->parameters().Add(PARAM_LABEL, "__mpd__", __FILE__, __LINE__);
    report->parameters().Add(PARAM_TYPE, PARAM_MPD, __FILE__, __LINE__);
    report->parameters().Add(PARAM_FILE_NAME, mpd_file_name, __FILE__, __LINE__);
    report->parameters().Add(PARAM_WRITE_MODE, mpd_write_mode, __FILE__, __LINE__);
    report->Validate(model);
    model->managers()->report()->AddObject(report);
    model->managers()->report()->Resume();
  }

  if (model->is_primary_thread_model() && model->global_configuration().create_free_parameter_output_file()) {
    string free_parameter_file_name  = model->global_configuration().get_free_parameter_output_file();
    string free_parameter_write_mode = model->global_configuration().free_parameter_write_mode();
    LOG_IMPORTANT() << "Setting '" << free_parameter_file_name << "' as the output file for the free parameters";
    model->managers()->report()->Pause();
    reports::EstimateValue* report = new reports::EstimateValue();
    report->set_block_type(PARAM_REPORT);
    report->set_defined_file_name(__FILE__);
    report->set_defined_line_number(__LINE__);
    report->parameters().Add(PARAM_LABEL, "__free_parameters__", __FILE__, __LINE__);
    report->parameters().Add(PARAM_TYPE, PARAM_ESTIMATE_VALUE, __FILE__, __LINE__);
    report->parameters().Add(PARAM_FILE_NAME, free_parameter_file_name, __FILE__, __LINE__);
    report->parameters().Add(PARAM_WRITE_MODE, free_parameter_write_mode, __FILE__, __LINE__);
    report->parameters().Add(PARAM_FORMAT, PARAM_NONE, __FILE__, __LINE__);
    report->Validate(model);
    model->managers()->report()->AddObject(report);
    model->managers()->report()->Resume();
  }
}

/**
 * @brief
 *
 * @return vector<double>
 */
vector<Double> Manager::lower_bounds() {
  vector<Double> result;
  for (auto* e : objects_) result.push_back(e->lower_bound());
  return result;
}

/**
 * @brief
 *
 * @return vector<double>
 */
vector<Double> Manager::upper_bounds() {
  vector<Double> result;
  for (auto* e : objects_) result.push_back(e->upper_bound());
  return result;
}

/**
 * Count how many of the estimates are enabled
 * and return the count
 *
 * @return The number of enabled estimates
 */
unsigned Manager::GetIsEstimatedCount() {
  unsigned count = 0;

  for (Estimate* estimate : objects_) {
    if (estimate->estimated())
      count++;
  }

  return count;
}

/**
 * Get a vector of pointers to the estimates
 *
 * @return The vector of enabled estimates
 */
vector<Estimate*> Manager::GetIsEstimated() {
  vector<Estimate*> result;
  for (Estimate* estimate : objects_) {
    if (estimate->estimated() & estimate->estimated_in_phasing()) {
      result.push_back(estimate);
    }
  }
  return result;
}

/**
 * Get a vector of pointers to the estimates in the objective function
 *
 * @return The vector of enables estimates
 */
vector<Estimate*> Manager::GetInObjectiveFunction() {
  vector<Estimate*> result;

  for (Estimate* estimate : objects_) {
    if (estimate->in_objective_function())
      result.push_back(estimate);
  }

  return result;
}

/**
 * @brief Get a vector of estimate pointers to estimates that
 * will be used during an MCMC run.
 *
 * Any estimates that have flagged mcmc_fixed as true will be returned. The covariance will be supplied for them so we need them
 * or having matching lower and upper bounds will not be returned
 *
 * @return vector<Estimate*>
 */
vector<Estimate*> Manager::GetIsMCMCd() {
  vector<Estimate*> result;

  for (Estimate* estimate : objects_) {
    if (estimate->estimated() && !math::IsEqual(estimate->lower_bound(), estimate->upper_bound()))
      result.push_back(estimate);
  }

  return result;
}

/**
 * Clear all objects
 */
void Manager::Clear() {
  objects_.clear();
  creators_.clear();
}

/**
 * This method looks for estimates that match the parameter
 *
 * First it will check all of the parameters parent info object
 * (the object that created them) for matches. If it does not
 * find a match then it will re-loop through the estimates
 * looking for a specific parameter there.
 *
 * This is to prioritise matches to what was
 * defined explicitly in the configuration file. As each @estimate
 * block in the file can create multiple Estimate objects,
 * this way will be prioritised first before looking at what
 * actual estimates were created.
 *
 * @param parameter The parameter the estimate is targeting
 * @return True if found, false otherwise
 */
bool Manager::HasEstimate(const string& parameter) {
  for (Estimate* estimate : objects_) {
    if (estimate->parameter() == parameter)
      return true;
  }

  for (Estimate* estimate : objects_) {
    if (estimate->creator_parameter() == parameter)
      return true;
  }

  return false;
}

/**
 * This method enables all estimates that match either the parameter
 * or the parent info matches the parameter
 *
 * @param parameter The parameter to match against the estimate and parent info
 */
void Manager::FlagIsEstimated(const string& parameter) {
  for (Estimate* estimate : objects_) {
    if (estimate->creator_parameter() == parameter)
      estimate->set_estimated(true);
    else if (estimate->parameter() == parameter)
      estimate->set_estimated(true);
  }
}

/**
 * This method disables all estimates that match either the parameter
 * or the parent info matches the parameter.
 *
 * @param parameter The parameter to match against the estimate and parent info
 */
void Manager::UnFlagIsEstimated(const string& parameter) {
  for (Estimate* estimate : objects_) {
    if (estimate->creator_parameter() == parameter)
      estimate->set_estimated(false);
    else if (estimate->parameter() == parameter)
      estimate->set_estimated(false);
  }
}

/**
 * This method returns a pointer to the estimate
 *
 * @param parameter The name of the estimate
 * @return The pointer to the Estimate
 */
Estimate* Manager::GetEstimate(const string& parameter) {
  for (Estimate* estimate : objects_) {
    if (estimate->parameter() == parameter)
      return estimate;
  }
  return nullptr;
}

/**
 * This method returns a pointer to the estimate for the label
 *
 * @param label The label of the estimate
 * @returns The pointer to the Estimate
 */
Estimate* Manager::GetEstimateByLabel(const string& label) {
  LOG_FINEST() << "Checking for estimate by label " << label;
  Estimate* result = nullptr;

  unsigned count = 0;
  for (Estimate* estimate : objects_) {
    if (estimate->label() == label) {
      result = estimate;
      ++count;
    }
  }
  if (count > 1)
    LOG_ERROR() << "Expected one parameter with the label " << label << " but found " << count;

  return result;
}

/**
 * This method returns a vector of pointers to the estimates for the label
 *
 * @param label The label of the estimates
 * @returns The pointer to the vector of Estimates
 */
vector<Estimate*> Manager::GetEstimatesByLabel(const string& label) {
  LOG_FINEST() << "Checking for estimate by label " << label;
  vector<Estimate*> result;

  unsigned count = 0;
  for (Estimate* estimate : objects_) {
    if (estimate->label() == label) {
      result.push_back(estimate);
      ++count;
    }
  }

  if (count > 1)
    LOG_FINEST() << "There were " << count << " parameters retrieved in the @estimate with label " << label;

  return result;
}

/**
 * This method enables all estimates who are part of the current phase or a prior phase
 *
 * @param phase The current estimate phase
 */
void Manager::SetActivePhase(unsigned phase) {
  for (auto estimate : objects_) {
    if (estimate->phase() <= phase) {
      estimate->set_estimated_in_phasing(true);
      LOG_FINEST() << "@estimate " << estimate->label() << " is enabled for phase " << phase;
    } else {
      estimate->set_estimated_in_phasing(false);
      LOG_FINEST() << "@estimate " << estimate->label() << " is disabled for phase " << phase;
    }
  }
}

/**
 * This method searches over all estimates and calculate how many phases exist across all estimates.
 * It will also apply a business rule, such as there must be consecutive phases, e.g., 1,2,3 not 1,3
 *
 * @param phase The current estimate phase
 */
unsigned Manager::GetNumberOfPhases() {
  vector<unsigned> store_unique_phases = {1};
  for (auto estimate : objects_) {
    unsigned current_phase = estimate->phase();
    // store unique phase numbers
    if (std::find(store_unique_phases.begin(), store_unique_phases.end(), current_phase) == store_unique_phases.end()) {
      store_unique_phases.push_back(current_phase);
      LOG_FINE() << "storing phase = " << current_phase;
    }
  }
  // Now check that there is a consecutive sequence.
  unsigned max = *max_element(store_unique_phases.begin(), store_unique_phases.end());
  LOG_FINE() << "found max = " << max << " iterations";
  for (unsigned i = 1; i <= max; ++i) {
    if (std::find(store_unique_phases.begin(), store_unique_phases.end(), i) == store_unique_phases.end()) {
      LOG_WARNING() << "Could not find estimation phase " << i << ", but found estimation phase " << max
                    << ". Specify consecutive phases, e.g., 1, 2, 3 and 4, not 1 and 4. Please check the specification of the estimation phases for the @estimates";
    }
  }
  return max;
}

} /* namespace estimates */
} /* namespace niwa */
