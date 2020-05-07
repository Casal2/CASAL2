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
#include "Observations/Observation.h"
#include "Utilities/DoubleCompare.h"

// namespaces
namespace niwa {
namespace reports {

namespace obs = niwa::observations;
namespace dc = niwa::utilities::doublecompare;

/**
 * Default constructor
 */
Observation::Observation(Model* model) : Report(model) {
  LOG_TRACE();
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection | RunMode::kSimulation | RunMode::kEstimation | RunMode::kProfiling);
  model_state_ = (State::Type)(State::kIterationComplete);

  parameters_.Bind<string>(PARAM_OBSERVATION, &observation_label_, "The observation label", "");
  parameters_.Bind<bool>(PARAM_NORMALISED_RESIDUALS, &normalised_resids_, "Print Normalised Residuals?", "", false);
  parameters_.Bind<bool>(PARAM_PEARSONS_RESIDUALS, &pearson_resids_, "Print Pearsons Residuals?", "", false);
}

/**
 * Build
 */
void Observation::DoBuild() {
  LOG_TRACE();
  vector<string> pearson_likelihoods = {PARAM_BINOMIAL, PARAM_MULTINOMIAL,PARAM_LOGNORMAL,PARAM_NORMAL,PARAM_BINOMIAL_APPROX};
  vector<string> normalised_likelihoods = {PARAM_LOGNORMAL,PARAM_NORMAL};

  observation_ = model_->managers().observation()->GetObservation(observation_label_);
  if (!observation_) {
    auto observations = model_->managers().observation()->objects();
    for (auto observation : observations)
      cout << observation->label() << endl;
    LOG_ERROR_P(PARAM_OBSERVATION) << "The observation label (" << observation_label_ << ") was not found.";
  }

  if (pearson_resids_) {
    if(std::find(pearson_likelihoods.begin(), pearson_likelihoods.end(), observation_->likelihood()) == pearson_likelihoods.end()) {
       LOG_ERROR_P(PARAM_PEARSONS_RESIDUALS) << "The likelihood associated with this observation is " << observation_->likelihood()
         << ". Pearsons residuals can be calculated only for the likelihoods binomial, multinomial, lognormal, normal, binomal_approx";
    }
  }
  if (normalised_resids_) {
    if(std::find(normalised_likelihoods.begin(), normalised_likelihoods.end(), observation_->likelihood()) == normalised_likelihoods.end()) {
       LOG_ERROR_P(PARAM_NORMALISED_RESIDUALS) << "The likelihood associated with this observation is " << observation_->likelihood()
         << ". Normalised residuals can be calculated only for the likelihoods lognormal, lognormal_with_Q, normal";
    }
  }

}

/**
 *  Execute the report
 */
void Observation::DoExecute() {
  cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
  cache_ << "observation_type: " << observation_->type() << "\n";
  cache_ << "likelihood: " << observation_->likelihood() << "\n";
  cache_ << "Values " <<REPORT_R_DATAFRAME <<"\n";
  map<unsigned, vector<obs::Comparison>>& comparisons = observation_->comparisons();
  if(pearson_resids_ && !normalised_resids_) {
    LOG_FINEST() << "calculating Pearsons residuals for observation " << label_ << " with likelihood type " << observation_->likelihood();
    // reporting pearsons residuals
    cache_ << "year category age length observed expected residual error_value process_error adjusted_error score pearsons_residuals\n";
    Double resid;
    for (auto iter = comparisons.begin(); iter != comparisons.end(); ++iter) {
      for (obs::Comparison comparison : iter->second) {
        if(observation_->likelihood() == PARAM_BINOMIAL){
          resid =(comparison.observed_ - comparison.expected_) / sqrt((dc::ZeroFun(comparison.expected_, comparison.delta_) * (1 - dc::ZeroFun(comparison.expected_, comparison.delta_))) / comparison.adjusted_error_);
        } else if (observation_->likelihood() == PARAM_MULTINOMIAL) {
            resid = (comparison.observed_ - comparison.expected_) / sqrt((dc::ZeroFun(comparison.expected_, comparison.delta_) * (1 - dc::ZeroFun(comparison.expected_, comparison.delta_))) / comparison.adjusted_error_);
        } else if (observation_->likelihood() == PARAM_BINOMIAL_APPROX) {
            resid = (comparison.observed_ - comparison.expected_) / sqrt(((comparison.expected_ + comparison.delta_) * (1 -comparison.expected_ + comparison.delta_)) / comparison.adjusted_error_);
        } else if (observation_->likelihood() == PARAM_LOGNORMAL) {
          resid = (comparison.observed_ - comparison.expected_) / (comparison.expected_ * comparison.adjusted_error_);
        } else if (observation_->likelihood() == PARAM_NORMAL) {
          resid = (comparison.observed_ - comparison.expected_) / (comparison.expected_ * comparison.adjusted_error_);
        } else {
          LOG_CODE_ERROR() << "Unknown coded likelihood type should be dealt with in DoBuild(). If the Pearsons residual is unknown"
            << " for this likelihood, set pearsons_residual to 'false'";
        }
        cache_ << iter->first << " " << comparison.category_ << " " << comparison.age_ << " " << comparison.length_ << " " << comparison.observed_ << " " << AS_VALUE(comparison.expected_)
          << " " << comparison.observed_ - AS_VALUE(comparison.expected_) << " " << comparison.error_value_ << " " <<AS_VALUE(comparison.process_error_) << " "
          << AS_VALUE(comparison.adjusted_error_) << " " << AS_VALUE(comparison.score_) << " " << AS_VALUE(resid) << "\n";
      }
    }
  } else if (normalised_resids_ && !pearson_resids_) {
    LOG_FINEST() << "calculating normalised residuals for observation " << label_ << " with likelihood type " << observation_->likelihood();
    // reporting normalised residuals
    cache_ << "year category age length observed expected residual error_value process_error adjusted_error score normalised_residuals\n";
    Double resid;
    for (auto iter = comparisons.begin(); iter != comparisons.end(); ++iter) {
      for (obs::Comparison comparison : iter->second) {
        if (observation_->likelihood() == PARAM_LOGNORMAL) {
          Double sigma =  sqrt(log(1 + comparison.adjusted_error_ * comparison.adjusted_error_));
          resid = (log(comparison.observed_ / comparison.expected_) + 0.5 * sigma * sigma) / sigma;
        } else if (observation_->likelihood() == PARAM_NORMAL) {
          resid =  (comparison.observed_ - comparison.expected_) / (comparison.expected_ * comparison.adjusted_error_);
        } else {
          LOG_CODE_ERROR() << "Unknown coded likelihood type should be dealt with in DoBuild(). If the normalised residual is unknown"
            << " for this likelihood, set normalised_residual to 'false'";
        }
        cache_ << iter->first << " " << comparison.category_ << " " << comparison.age_ << " " << comparison.length_ << " " << comparison.observed_ << " " << AS_VALUE(comparison.expected_)
          << " " << comparison.observed_ - AS_VALUE(comparison.expected_) << " " << comparison.error_value_ << " " <<AS_VALUE(comparison.process_error_) << " "
          << AS_VALUE(comparison.adjusted_error_) << " " << AS_VALUE(comparison.score_) << " " << AS_VALUE(resid) << "\n";
      }
    }
  } else if (normalised_resids_ && pearson_resids_){
    LOG_FINEST() << "calculating normalised and Pearsons residuals for observation " << label_ << " with likelihood type " << observation_->likelihood();
    // report both normalised residuals and pearsons residuals
    Double pearson_resid, normalised_resid;
    cache_ << "year category age length observed expected residual error_value process_error adjusted_error score pearsons_residuals normalised_residuals\n";
    for (auto iter = comparisons.begin(); iter != comparisons.end(); ++iter) {
      for (obs::Comparison comparison : iter->second) {
        if (observation_->likelihood() == PARAM_LOGNORMAL) {
          Double sigma =  sqrt(log(1 + comparison.adjusted_error_ * comparison.adjusted_error_));
          normalised_resid = (log(comparison.observed_ / comparison.expected_) + 0.5 * sigma * sigma) / sigma;
          pearson_resid = (comparison.observed_ - comparison.expected_) / (comparison.expected_ * comparison.adjusted_error_);
        } else if (observation_->likelihood() == PARAM_NORMAL) {
          normalised_resid = (comparison.observed_ - comparison.expected_) / (comparison.expected_ * comparison.adjusted_error_);
          pearson_resid = (comparison.observed_ - comparison.expected_) / (comparison.expected_ * comparison.adjusted_error_);
        } else {
          LOG_CODE_ERROR() << "Unknown coded likelihood type should be dealt with in DoBuild(). If the normalised or Pearsons residual is unknown"
            << " for this likelihood, set normalised_residual to 'false' and pearsons_residual to 'false'";
        }
        cache_ << iter->first << " " << comparison.category_ << " " << comparison.age_ << " " << comparison.length_ << " " << comparison.observed_ << " " << AS_VALUE(comparison.expected_)
          << " " << comparison.observed_ - AS_VALUE(comparison.expected_) << " " << comparison.error_value_ << " " <<AS_VALUE(comparison.process_error_)  << " "
          << AS_VALUE(comparison.adjusted_error_) << " " << AS_VALUE(comparison.score_) << " " << AS_VALUE(pearson_resid) << " " << AS_VALUE(normalised_resid) << "\n";
      }
    }
  } else {
    // report raw residuals
    cache_ << "year category age length observed expected residual error_value process_error adjusted_error score\n";
    for (auto iter = comparisons.begin(); iter != comparisons.end(); ++iter) {
      for (obs::Comparison comparison : iter->second) {
        cache_ << iter->first << " " << comparison.category_ << " " << comparison.age_ << " " << comparison.length_ << " " << comparison.observed_ << " " << AS_VALUE(comparison.expected_)
          << " " << comparison.observed_ - AS_VALUE(comparison.expected_) << " " << comparison.error_value_ << " " << AS_VALUE(comparison.process_error_)  << " "
          << AS_VALUE(comparison.adjusted_error_) << " " << AS_VALUE(comparison.score_) << "\n";
      }
    }
  }

  ready_for_writing_ = true;
}

/**
 *  Execute the tabular report
 */
void Observation::DoExecuteTabular() {
  map<unsigned, vector<obs::Comparison>>& comparisons = observation_->comparisons();
  if (first_run_) {
    first_run_ = false;
    cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
    cache_ << "observation_type: " << observation_->type() << "\n";
    cache_ << "likelihood: " << observation_->likelihood() << "\n";
    cache_ << "values " << REPORT_R_DATAFRAME << "\n";
    string bin,year,label;

    /**
     *  Generate header
     */
    // Generate labels for the fits
    for (auto iter = comparisons.begin(); iter != comparisons.end(); ++iter) {
      if (!utilities::To<unsigned, string>(iter->first, year))
        LOG_CODE_ERROR() << "Could not convert the value " << iter->first << " to a string for storage in the tabular report";
      for (obs::Comparison comparison : iter->second) {
        if((comparison.length_ == 0) && (comparison.age_ == 0)) {
          // Biomass/abundance
          bin = "1";
        } else if ((comparison.length_ == 0) && (comparison.age_ != 0)) {
          // age based observation
          if (!utilities::To<Double, string>(comparison.age_, bin))
            LOG_CODE_ERROR() << "Could not convert the value " << comparison.age_ << " to a string for storage in the tabular report";
        } else if ((comparison.length_ != 0) && (comparison.age_ == 0)) {
          // length based observation
          if (!utilities::To<Double, string>(comparison.length_, bin))
            LOG_CODE_ERROR() << "Could not convert the value " << comparison.length_ << " to a string for storage in the tabular report";
        } else {
          LOG_ERROR() << "There is no tabular report for an observation that has a structured comparison as in observation "
            << observation_label_;
        }
        label = observation_label_ + ".fits" + "[" + year + "][" + bin + "]";
        cache_ << label << " ";
      }
    }

    // Generate labels for the obs
    for (auto iter = comparisons.begin(); iter != comparisons.end(); ++iter) {
      if (!utilities::To<unsigned, string>(iter->first, year))
        LOG_CODE_ERROR() << "Could not convert the value " << iter->first << " to a string for storage in the tabular report";
      for (obs::Comparison comparison : iter->second) {
        if((comparison.length_ == 0) && (comparison.age_ == 0)) {
          // Biomass/abundance
        } else if ((comparison.length_ == 0) && (comparison.age_ != 0)) {
          // age based observation
          if (!utilities::To<Double, string>(comparison.age_, bin))
            LOG_CODE_ERROR() << "Could not convert the value " << comparison.age_ << " to a string for storage in the tabular report";
        } else if ((comparison.length_ != 0) && (comparison.age_ == 0)) {
          // length based observation
          if (!utilities::To<Double, string>(comparison.length_, bin))
            LOG_CODE_ERROR() << "Could not convert the value " << comparison.length_ << " to a string for storage in the tabular report";
        } else {
          LOG_ERROR() << "There is no tabular report for an observation that has a structured comparison as in observation "
            << observation_label_;
        }
        label = observation_label_ + ".observed" + "[" + year + "][" + bin + "]";
        cache_ << label << " ";
      }
    }

    // Generate labels for the resids
    for (auto iter = comparisons.begin(); iter != comparisons.end(); ++iter) {
      if (!utilities::To<unsigned, string>(iter->first, year))
        LOG_CODE_ERROR() << "Could not convert the value " << iter->first << " to a string for storage in the tabular report";
      for (obs::Comparison comparison : iter->second) {
        if((comparison.length_ == 0) && (comparison.age_ == 0)) {
          // Biomass/abundance
        } else if ((comparison.length_ == 0) && (comparison.age_ != 0)) {
          // age based observation
          if (!utilities::To<Double, string>(comparison.age_, bin))
            LOG_CODE_ERROR() << "Could not convert the value " << comparison.age_ << " to a string for storage in the tabular report";
        } else if ((comparison.length_ != 0) && (comparison.age_ == 0)) {
          // length based observation
          if (!utilities::To<Double, string>(comparison.length_, bin))
            LOG_CODE_ERROR() << "Could not convert the value " << comparison.length_ << " to a string for storage in the tabular report";
        } else {
          LOG_ERROR() << "There is no tabular report for an observation that has a structured comparison as in observation "
            << observation_label_;
        }
        label = observation_label_ + ".residuals" + "[" + year + "][" + bin + "]";
        cache_ << label << " ";
      }
    }

    if (pearson_resids_) {
      // Generate labels for the pearsons resids
      for (auto iter = comparisons.begin(); iter != comparisons.end(); ++iter) {
        if (!utilities::To<unsigned, string>(iter->first, year))
          LOG_CODE_ERROR() << "Could not convert the value " << iter->first << " to a string for storage in the tabular report";
        for (obs::Comparison comparison : iter->second) {
          if((comparison.length_ == 0) && (comparison.age_ == 0)) {
            // Biomass/abundance
          } else if ((comparison.length_ == 0) && (comparison.age_ != 0)) {
            // age based observation
            if (!utilities::To<Double, string>(comparison.age_, bin))
              LOG_CODE_ERROR() << "Could not convert the value " << comparison.age_ << " to a string for storage in the tabular report";
          } else if ((comparison.length_ != 0) && (comparison.age_ == 0)) {
            // length based observation
            if (!utilities::To<Double, string>(comparison.length_, bin))
              LOG_CODE_ERROR() << "Could not convert the value " << comparison.length_ << " to a string for storage in the tabular report";
          } else {
              LOG_ERROR() << "There is no tabular report for an observation that has a structured comparison as in observation "
                << observation_label_;
          }
          label = observation_label_ + ".pearson_residuals" + "[" + year + "][" + bin + "]";
          cache_ << label << " ";
        }
      }
    }

    if (normalised_resids_) {
      // Generate labels for the normalised resids
      for (auto iter = comparisons.begin(); iter != comparisons.end(); ++iter) {
        if (!utilities::To<unsigned, string>(iter->first, year))
          LOG_CODE_ERROR() << "Could not convert the value " << iter->first << " to a string for storage in the tabular report";
        for (obs::Comparison comparison : iter->second) {
          if((comparison.length_ == 0) && (comparison.age_ == 0)) {
            // Biomass/abundance
          } else if ((comparison.length_ == 0) && (comparison.age_ != 0)) {
            // age based observation
            if (!utilities::To<Double, string>(comparison.age_, bin))
              LOG_CODE_ERROR() << "Could not convert the value " << comparison.age_ << " to a string for storage in the tabular report";
          } else if ((comparison.length_ != 0) && (comparison.age_ == 0)) {
            // length based observation
            if (!utilities::To<Double, string>(comparison.length_, bin))
              LOG_CODE_ERROR() << "Could not convert the value " << comparison.length_ << " to a string for storage in the tabular report";
          } else {
              LOG_ERROR() << "There is no tabular report for an observation that has a structured comparison as in observation "
                << observation_label_;
          }
          label = observation_label_ + ".normalised_residuals" + "[" + year + "][" + bin + "]";
          cache_ << label << " ";
        }
      }
    }
    cache_ << "\n";
  }

  /**
   *  Print Values
   */
  // Print fits
  for (auto iter = comparisons.begin(); iter != comparisons.end(); ++iter) {
    for (obs::Comparison comparison : iter->second) {
      cache_ << AS_VALUE(comparison.expected_) << " ";
    }
  }

  // Print obs
  for (auto iter = comparisons.begin(); iter != comparisons.end(); ++iter) {
    for (obs::Comparison comparison : iter->second) {
      cache_ << comparison.observed_ << " ";
    }
  }

  // Print resids
  Double resid = 0.0;
  for (auto iter = comparisons.begin(); iter != comparisons.end(); ++iter) {
    for (obs::Comparison comparison : iter->second) {
      resid = comparison.observed_ - comparison.expected_;
      cache_ << AS_VALUE(resid) << " ";
    }
  }

  if (pearson_resids_) {
    // Generate labels for the pearsons resids
    Double resid;
    for (auto iter = comparisons.begin(); iter != comparisons.end(); ++iter) {
      for (obs::Comparison comparison : iter->second) {
        if(observation_->likelihood() == PARAM_BINOMIAL){
          resid =(comparison.observed_ - comparison.expected_) / sqrt((dc::ZeroFun(comparison.expected_, comparison.delta_) * (1 - dc::ZeroFun(comparison.expected_, comparison.delta_))) / comparison.adjusted_error_);
        } else if (observation_->likelihood() == PARAM_MULTINOMIAL) {
            resid = (comparison.observed_ - comparison.expected_) / sqrt((dc::ZeroFun(comparison.expected_, comparison.delta_) * (1 - dc::ZeroFun(comparison.expected_, comparison.delta_))) / comparison.adjusted_error_);
        } else if (observation_->likelihood() == PARAM_BINOMIAL_APPROX) {
            resid = (comparison.observed_ - comparison.expected_) / sqrt(((comparison.expected_ + comparison.delta_) * (1 -comparison.expected_ + comparison.delta_)) / comparison.adjusted_error_);
        } else if (observation_->likelihood() == PARAM_LOGNORMAL) {
          resid = (comparison.observed_ - comparison.expected_) / (comparison.expected_ * comparison.adjusted_error_);
        } else if (observation_->likelihood() == PARAM_NORMAL) {
          resid = (comparison.observed_ - comparison.expected_) / (comparison.expected_ * comparison.adjusted_error_);
        } else {
          LOG_CODE_ERROR() << "Unknown coded likelihood type should be dealt with in DoBuild(). If the Pearsons residual is unknown"
            << " for this likelihood, set pearsons_residual to 'false'";
        }
        cache_ << AS_VALUE(resid) << " ";
      }
    }
  }

  if (normalised_resids_) {
    // Generate labels for the normalised resids
    Double resid;
    for (auto iter = comparisons.begin(); iter != comparisons.end(); ++iter) {
      for (obs::Comparison comparison : iter->second) {
        if (observation_->likelihood() == PARAM_LOGNORMAL) {
          Double sigma =  sqrt(log(1 + comparison.adjusted_error_ * comparison.adjusted_error_));
          resid = (log(comparison.observed_ / comparison.expected_) + 0.5 * sigma * sigma) / sigma;
        } else if (observation_->likelihood() == PARAM_NORMAL) {
          resid =  (comparison.observed_ - comparison.expected_) / (comparison.expected_ * comparison.adjusted_error_);
        } else {
          LOG_CODE_ERROR() << "Unknown coded likelihood type should be dealt with in DoBuild(). If the normalised residual is unknown"
            << " for this likelihood, set normalised_residual to 'false'";
        }
        cache_ << AS_VALUE(resid) << " ";
      }
    }
  }

  cache_ << "\n";
}

/**
 *  Finalise the tabular report
 */
void Observation::DoFinaliseTabular() {
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
