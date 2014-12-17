/**
 * @file Observation.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 21/01/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Observation.h"
#include "Observations/Manager.h"

// namespaces
namespace niwa {
namespace reports {

namespace obs = niwa::observations;

/**
 *
 */
Observation::Observation() {
  LOG_TRACE();
  model_state_ = State::kPostExecute;
  run_mode_    = RunMode::kBasic;

  parameters_.Bind<string>(PARAM_OBSERVATION, &observation_label_, "Observation label", "");
}

/**
 *
 */
void Observation::DoBuild() {
  LOG_TRACE();

  observation_ = observations::Manager::Instance().GetObservation(observation_label_);
  if (!observation_) {
    vector<ObservationPtr> obs = observations::Manager::Instance().GetObjects();
    for (ObservationPtr ob_ptr : obs)
      cout << ob_ptr->label() << endl;
    LOG_ERROR(parameters_.location(PARAM_OBSERVATION) << " (" << observation_label_ << ") could not be found. Have you defined it?");
  }
}

/**
 *
 */
void Observation::DoExecute() {
  cache_ << CONFIG_ARRAY_START << label_ << CONFIG_ARRAY_END << "\n";

  cache_ << PARAM_OBSERVATION << ": " << observation_->label() << "\n";
  cache_ << "Comparisons: \n";

  cache_ << "year category age observed expected residual error_value score\n";

  map<unsigned, vector<obs::Comparison> >& comparisons = observation_->comparisons();
  for (auto iter = comparisons.begin(); iter != comparisons.end(); ++iter) {
    for (obs::Comparison comparison : iter->second) {

      cache_ << iter->first << " " << comparison.category_ << " " << comparison.age_ << " " << AS_DOUBLE(comparison.observed_) << " " << AS_DOUBLE(comparison.expected_)
          << " " << AS_DOUBLE(comparison.observed_) - AS_DOUBLE(comparison.expected_) << " " << AS_DOUBLE(comparison.error_value_) << " "
          << AS_DOUBLE(comparison.score_) << "\n";
    }
  }

  cache_ << CONFIG_END_REPORT << "\n" << endl;
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
