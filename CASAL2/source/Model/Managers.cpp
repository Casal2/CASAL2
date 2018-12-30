/**
 * @file Managers.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/09/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "Managers.h"

#include "Model/Model.h"
#include "AdditionalPriors/Manager.h"
#include "AgeingErrors/Manager.h"
#include "AgeLengths/Manager.h"
#include "AgeWeights/Manager.h"
#include "Asserts/Manager.h"
#include "Catchabilities/Manager.h"
#include "DerivedQuantities/Manager.h"
#include "Estimables/Estimables.h"
#include "Estimates/Manager.h"
#include "EstimateTransformations/Manager.h"
#include "InitialisationPhases/Manager.h"
#include "LengthWeights/Manager.h"
#include "Likelihoods/Manager.h"
#include "MCMCs/Manager.h"
#include "Minimisers/Manager.h"
#include "Observations/Manager.h"
#include "Penalties/Manager.h"
#include "Processes/Manager.h"
#include "Profiles/Manager.h"
#include "Projects/Manager.h"
#include "Reports/Manager.h"
#include "Selectivities/Manager.h"
#include "Simulates/Manager.h"
#include "TimeSteps/Manager.h"
#include "TimeVarying/Manager.h"

// namespaces
namespace niwa {

/**
 * Default constructor
 */
Managers::Managers(Model* model) {
  LOG_TRACE();

  model_ = model;

  additional_prior_       = new additionalpriors::Manager();
  ageing_error_           = new ageingerrors::Manager();
  age_length_             = new agelengths::Manager();
  age_weight_             = new ageweights::Manager();
  assert_                 = new asserts::Manager();
  catchability_           = new catchabilities::Manager();
  derived_quantity_       = new derivedquantities::Manager();
  estimables_             = new Estimables(model_);
  estimate_               = new estimates::Manager();
  estimate_transformation_ = new estimatetransformations::Manager();
  initialisation_phase_   = new initialisationphases::Manager();
  length_weight_          = new lengthweights::Manager();
  likelihood_             = new likelihoods::Manager();
  mcmc_                   = new mcmcs::Manager();
  minimiser_              = new minimisers::Manager();
  observation_            = new observations::Manager();
  penalty_                = new penalties::Manager();
  process_                = new processes::Manager();
  profile_                = new profiles::Manager();
  project_                = new projects::Manager();
  report_                 = new reports::Manager(model_);
  selectivity_            = new selectivities::Manager();
  simulate_               = new simulates::Manager();
  time_step_              = new timesteps::Manager();
  time_varying_           = new timevarying::Manager();
}

/**
 * Destructor
 */
Managers::~Managers() {
  delete additional_prior_;
  delete ageing_error_;
  delete age_length_;
  delete age_weight_;
  delete assert_;
  delete catchability_;
  delete derived_quantity_;
  delete estimables_;
  delete estimate_;
  delete estimate_transformation_;
  delete initialisation_phase_;
  delete length_weight_;
  delete likelihood_;
  delete mcmc_;
  delete minimiser_;
  delete observation_;
  delete penalty_;
  delete process_;
  delete profile_;
  delete project_;
  delete report_;
  delete selectivity_;
  delete simulate_;
  delete time_step_;
  delete time_varying_;
}

void Managers::Validate() {
  LOG_TRACE();
  time_step_->Validate(model_);
  initialisation_phase_->Validate();
  process_->Validate(model_); // Needs to go before estimate for the situation where there is an @estimate block

  additional_prior_->Validate();
  ageing_error_->Validate();
  age_length_->Validate();
  age_weight_->Validate();
  assert_->Validate();
  catchability_->Validate();
  derived_quantity_->Validate();
  estimate_transformation_->Validate();
  length_weight_->Validate();
  likelihood_->Validate();
  mcmc_->Validate(model_);
  minimiser_->Validate(model_);
  observation_->Validate();
  penalty_->Validate();
  profile_->Validate();
  project_->Validate();
  report_->Validate();
  selectivity_->Validate();
  simulate_->Validate();
  time_varying_->Validate();

  estimate_->Validate(model_);
}

void Managers::Build() {
  LOG_TRACE();
  time_step_->Build();
  initialisation_phase_->Build(model_);
  process_->Build(); // To handle BH Recruitment having ssb_offset available

  additional_prior_->Build();
  ageing_error_->Build();
  age_length_->Build();
  age_weight_->Build();
  assert_->Build();
  catchability_->Build();
  derived_quantity_->Build();
  length_weight_->Build();
  likelihood_->Build();
  mcmc_->Build();
  minimiser_->Build();
  observation_->Build();
  penalty_->Build();
  profile_->Build();
  project_->Build(model_);
  selectivity_->Build();
  simulate_->Build();
  time_varying_->Build();

  estimate_->Build(model_);
  estimate_transformation_->Build();

  report_->Build();
}

void Managers::Reset() {
  LOG_TRACE();
  additional_prior_->Reset();
  ageing_error_->Reset();
  age_length_->Reset();
  age_weight_->Reset();
  assert_->Reset();
  catchability_->Reset();
  derived_quantity_->Reset();
  estimate_->Reset();
  estimate_transformation_->Reset();
  initialisation_phase_->Reset();
  length_weight_->Reset();
  likelihood_->Reset();
  if (model_->run_mode() == RunMode::kMCMC || model_->run_mode() == RunMode::kEstimation || model_->run_mode() == RunMode::kProfiling) {
    mcmc_->Reset();
    minimiser_->Reset();
  }

  observation_->Reset();
  penalty_->Reset();
  process_->Reset();
  profile_->Reset();
  project_->Reset();
  report_->Reset();
  selectivity_->Reset();
  simulate_->Reset();
  time_step_->Reset();
  time_varying_->Reset();
}

} /* namespace niwa */
