/**
 * @file Observation.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 21/01/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */

// headers
#include "Observation.h"
#include "Observations/Manager.h"
#include "Observations/Observation.h"

// namespaces
namespace niwa {
namespace reports {

namespace obs = niwa::observations;

/**
 *
 */
Observation::Observation(Model* model) : Report(model) {
  LOG_TRACE();
  model_state_ = State::kPostExecute;
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kSimulation);

  parameters_.Bind<string>(PARAM_OBSERVATION, &observation_label_, "Observation label", "");
}

/**
 *
 */
void Observation::DoBuild() {
  LOG_TRACE();

  observation_ = model_->managers().observation()->GetObservation(observation_label_);
  if (!observation_) {
    auto observations = model_->managers().observation()->objects();
    for (auto observation : observations)
      cout << observation->label() << endl;
    LOG_ERROR_P(PARAM_OBSERVATION) << " (" << observation_label_ << ") could not be found. Have you defined it?";
  }
}

/**
 *
 */
void Observation::DoExecute() {
	/*
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
	*/
  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";

  cache_ << "Comparisons " <<REPORT_R_DATAFRAME <<"\n";

  cache_ << "year category age length observed expected residual error_value score\n";

  map<unsigned, vector<obs::Comparison> >& comparisons = observation_->comparisons();
  for (auto iter = comparisons.begin(); iter != comparisons.end(); ++iter) {
    for (obs::Comparison comparison : iter->second) {

      cache_ << iter->first << " " << comparison.category_ << " " << comparison.age_ << " " << comparison.length_ << " " << AS_DOUBLE(comparison.observed_) << " " << AS_DOUBLE(comparison.expected_)
	         << " " << AS_DOUBLE(comparison.observed_) - AS_DOUBLE(comparison.expected_) << " " << AS_DOUBLE(comparison.error_value_) << " "
	         << AS_DOUBLE(comparison.score_) << "\n";
	}
  }
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
