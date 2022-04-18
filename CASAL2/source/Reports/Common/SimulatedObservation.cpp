/**
 * @file SimulatedObservation.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 8/05/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "SimulatedObservation.h"

#include "../../Observations/Manager.h"
#include "../../Observations/Observation.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
SimulatedObservation::SimulatedObservation() {
  run_mode_    = RunMode::kSimulation;
  model_state_ = State::kIterationComplete;
  skip_tags_   = true;

  parameters_.Bind<string>(PARAM_OBSERVATION, &observation_label_, "The observation label", "", "");
}

/**
 * Validate object
 */
void SimulatedObservation::DoValidate(shared_ptr<Model> model) {
  if (observation_label_ == "")
    observation_label_ = label_;
}

/**
 * Build method
 */
void SimulatedObservation::DoBuild(shared_ptr<Model> model) {
  observation_ = model->managers()->observation()->GetObservation(observation_label_);
  if (!observation_) {
#ifndef TESTMODE
    LOG_WARNING() << "The report for " << PARAM_OBSERVATION << " with label '" << observation_label_ << "' was requested. This " << PARAM_OBSERVATION
                  << " was not found in the input configuration file and the report will not be generated";
#endif
    is_valid_ = false;
  }
}

/**
 * Execute method
 */
void SimulatedObservation::DoExecute(shared_ptr<Model> model) {
  if (!is_valid())
    return;

  cache_ << CONFIG_SECTION_SYMBOL << PARAM_OBSERVATION << " " << label_ << REPORT_EOL;
  bool                           biomass_abundance_obs = false;
  bool                           tag_recapture_obs = false;
  bool                           tag_recapture_for_growth = false;

  ParameterList&                 parameter_list        = observation_->parameters();
  const map<string, Parameter*>& parameters            = parameter_list.parameters();
  for (auto iter = parameters.begin(); iter != parameters.end(); ++iter) {
    if (iter->first == PARAM_LIKELIHOOD) {
      if (iter->second->values()[0] == PARAM_PSEUDO)
        cache_ << PARAM_LIKELIHOOD << " " << parameter_list.Get(PARAM_SIMULATED_OBSERVATION)->values()[0] << REPORT_EOL;
      else
        cache_ << PARAM_LIKELIHOOD << " " << iter->second->values()[0] << REPORT_EOL;

      continue;
    }

    if (iter->first == PARAM_OBS || iter->first == PARAM_ERROR_VALUE || iter->first == PARAM_LABEL || iter->first == PARAM_SIMULATION_LIKELIHOOD)
      continue;

    if (iter->second->values().size() > 0) {
      cache_ << iter->first << " ";
      const vector<string>& values = iter->second->values();
      for (string value : values) {
        if (iter->first == PARAM_TYPE)
          LOG_FINE() << "Type of observation simulating = " << value;
        if ((iter->first == PARAM_TYPE && value == PARAM_BIOMASS) || (iter->first == PARAM_TYPE && value == PARAM_ABUNDANCE)) {
          biomass_abundance_obs = true;
        }
        if ((iter->first == PARAM_TYPE && value == PARAM_TAG_RECAPTURE_BY_AGE) || (iter->first == PARAM_TYPE && value == PARAM_TAG_RECAPTURE_BY_LENGTH)) {
          tag_recapture_obs = true;
        }
        if (iter->first == PARAM_TYPE && value == PARAM_TAG_RECAPTURE_BY_LENGTH_FOR_GROWTH) {
          tag_recapture_for_growth = true;
        }
        cache_ << value << " ";
      }
      cache_ << REPORT_EOL;
    }
  }

  map<unsigned, vector<obs::Comparison> >& comparison = observation_->comparisons();
  // Print Observations

  if (biomass_abundance_obs) {
    // biomass obs
    cache_ << PARAM_TABLE << " " << PARAM_OBS << REPORT_EOL;

    for (auto iter = comparison.begin(); iter != comparison.end(); ++iter) {
      cache_ << iter->first << " ";
      for (obs::Comparison comparison : iter->second) 
        cache_ << comparison.observed_ << " " << comparison.error_value_;
      cache_ << REPORT_EOL;
    }
    cache_ << PARAM_END_TABLE << REPORT_EOL;
  } else if(tag_recapture_obs)  {
    // tag recapture 
    cache_ << PARAM_TABLE << " " << PARAM_RECAPTURED << REPORT_EOL;
    for (auto iter = comparison.begin(); iter != comparison.end(); ++iter) {
      cache_ << iter->first << " ";
      for (obs::Comparison comparison : iter->second) {
        cache_ << comparison.observed_ << " ";
      }
      cache_ << REPORT_EOL;
    }
    cache_ << PARAM_END_TABLE << REPORT_EOL;
  } else if(tag_recapture_for_growth)  {
    // tag recapture for growth
    cache_ << PARAM_TABLE << " " << PARAM_RECAPTURED << REPORT_EOL;
    for (auto iter = comparison.begin(); iter != comparison.end(); ++iter) {
      cache_ << iter->first << " ";
      for (obs::Comparison comparison : iter->second) {
        cache_ << comparison.observed_ << " ";
      }
      cache_ << REPORT_EOL;
    }
    cache_ << PARAM_END_TABLE << REPORT_EOL;
  } else {
    // proportion at age obs
    cache_ << PARAM_TABLE << " " << PARAM_OBS << REPORT_EOL;
    for (auto iter = comparison.begin(); iter != comparison.end(); ++iter) {
      cache_ << iter->first << " ";
      for (obs::Comparison comparison : iter->second) {
        cache_ << comparison.observed_ << " ";
      }
      cache_ << REPORT_EOL;
    }
    cache_ << PARAM_END_TABLE << REPORT_EOL;
  }

  // if not biomass or tag growth print error values
  if (!biomass_abundance_obs && !tag_recapture_for_growth) {
    if(tag_recapture_obs) {
      // tag recpat by length or age
      cache_ << PARAM_TABLE << " " << PARAM_SCANNED << REPORT_EOL;
      for (auto iter = comparison.begin(); iter != comparison.end(); ++iter) {
        cache_ << iter->first << " ";
        for (obs::Comparison comparison : iter->second) {
          cache_ << comparison.error_value_ << " ";
        }
        cache_ << REPORT_EOL;
      }
      cache_ << PARAM_END_TABLE << REPORT_EOL;
    } else {
      // proportion at age obs
      cache_ << PARAM_TABLE << " " << PARAM_ERROR_VALUES << REPORT_EOL;
      for (auto iter = comparison.begin(); iter != comparison.end(); ++iter) {
        cache_ << iter->first << " ";
        for (obs::Comparison comparison : iter->second) {
          cache_ << comparison.error_value_ << " ";
        }
        cache_ << REPORT_EOL;
      }
      cache_ << PARAM_END_TABLE << REPORT_EOL;
    }
  }
  cache_ << REPORT_EOL;
  ready_for_writing_ = true;
}

void SimulatedObservation::DoPrepareTabular(shared_ptr<Model> model) {
  LOG_INFO() << "Tabular mode for reports of type " << PARAM_SIMULATED_OBSERVATION << " has not been implemented";
}

} /* namespace reports */
} /* namespace niwa */
