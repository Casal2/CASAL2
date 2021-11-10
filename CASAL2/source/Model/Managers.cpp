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

#include <mutex>
#include <thread>
#include <vector>

#include "../AdditionalPriors/Manager.h"
#include "../AddressableInputLoader/AddressableInputLoader.h"
#include "../AddressableTransformations/Manager.h"
#include "../AgeLengths/Manager.h"
#include "../AgeWeights/Manager.h"
#include "../AgeingErrors/Manager.h"
#include "../Asserts/Manager.h"
#include "../Catchabilities/Manager.h"
#include "../Categories/Categories.h"
#include "../DerivedQuantities/Manager.h"
#include "../Estimates/Manager.h"
#include "../InitialisationPhases/Manager.h"
#include "../LengthWeights/Manager.h"
#include "../Likelihoods/Manager.h"
#include "../MCMCs/Manager.h"
#include "../Minimisers/Manager.h"
#include "../Model/Model.h"
#include "../Observations/Manager.h"
#include "../Penalties/Manager.h"
#include "../Processes/Manager.h"
#include "../Profiles/Manager.h"
#include "../Projects/Manager.h"
#include "../Reports/Manager.h"
#include "../Selectivities/Manager.h"
#include "../Simulates/Manager.h"
#include "../TimeSteps/Manager.h"
#include "../TimeVarying/Manager.h"

// namespaces
namespace niwa {
using std::vector;

using std::scoped_lock;
std::mutex Managers::lock_;

/**
 * Default constructor
 */
Managers::Managers(shared_ptr<Model> model) {
  LOG_TRACE();

  model_ = model;

  additional_prior_           = new additionalpriors::Manager();
  ageing_error_               = new ageingerrors::Manager();
  age_length_                 = new agelengths::Manager();
  age_weight_                 = new ageweights::Manager();
  assert_                     = new asserts::Manager();
  catchability_               = new catchabilities::Manager();
  derived_quantity_           = new derivedquantities::Manager();
  addressable_input_loader_   = new AddressableInputLoader(model_);
  estimate_                   = new estimates::Manager();
  addressable_transformation_ = new addressabletransformations::Manager();
  initialisation_phase_       = new initialisationphases::Manager();
  length_weight_              = new lengthweights::Manager();
  likelihood_                 = new likelihoods::Manager();
  observation_                = new observations::Manager();
  penalty_                    = new penalties::Manager();
  process_                    = new processes::Manager();
  profile_                    = new profiles::Manager();
  project_                    = new projects::Manager();
  selectivity_                = new selectivities::Manager();
  simulate_                   = new simulates::Manager();
  time_step_                  = new timesteps::Manager();
  time_varying_               = new timevarying::Manager();
}

/**
 * @brief This method will create some test
 * managers that won't exist because this class was instantiated via
 * the Model class and not the Runner class. This is a legacy
 * requirement for unit tests that haven't been migrated to the Runner
 * way of life.
 */
void Managers::create_test_managers() {
  minimiser_.reset(new minimisers::Manager());
  report_.reset(new reports::Manager());
  mcmc_.reset(new mcmcs::Manager());
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
  delete addressable_input_loader_;
  delete estimate_;
  delete addressable_transformation_;
  delete initialisation_phase_;
  delete length_weight_;
  delete likelihood_;
  delete observation_;
  delete penalty_;
  delete process_;
  delete profile_;
  delete project_;
  delete selectivity_;
  delete simulate_;
  delete time_step_;
  delete time_varying_;
}

/**
 *
 */
shared_ptr<mcmcs::Manager> Managers::mcmc() {
  std::scoped_lock l(lock_);
  if (!mcmc_)
    LOG_CODE_ERROR() << "(!mcmc_)";

  return mcmc_;
}

/**
 *
 */
shared_ptr<minimisers::Manager> Managers::minimiser() {
  std::scoped_lock l(lock_);
  if (!minimiser_)
    LOG_CODE_ERROR() << "(!minimiser_)";

  return minimiser_;
}

/**
 *
 */
shared_ptr<reports::Manager> Managers::report() {
  std::scoped_lock l(lock_);
  if (!report_)
    LOG_CODE_ERROR() << "(!report_)";

  return report_;
}

void Managers::Validate() {
  //	std::scoped_lock l(lock_);
  auto run_mode = model_->run_mode();
  LOG_TRACE();
  time_step_->Validate(model_);
  initialisation_phase_->Validate();
  process_->Validate(model_);  // Needs to go before estimate for the situation where there is an @estimate block

  addressable_input_loader_->Validate();
  additional_prior_->Validate();
  ageing_error_->Validate();
  age_length_->Validate();
  age_weight_->Validate();
  assert_->Validate();
  catchability_->Validate();
  derived_quantity_->Validate();
  length_weight_->Validate();
  likelihood_->Validate();
  if (mcmc_ && (run_mode == RunMode::kMCMC || run_mode == RunMode::kTesting))
    mcmc_->Validate(model_);
  if (minimiser_)
    minimiser_->Validate(model_);
  observation_->Validate();
  penalty_->Validate();
  profile_->Validate();
  project_->Validate();
  LOG_FINE() << "Validating Reports";
  report_->Validate(model_);
  LOG_FINE() << "Validating Reports..Done";
  selectivity_->Validate();
  simulate_->Validate();
  time_varying_->Validate();

  addressable_transformation_->Validate();  // needs to be at the after all other classes, but before Estimates
  estimate_->Validate(model_);
  LOG_TRACE();
}

void Managers::Build() {
  auto run_mode = model_->run_mode();
  //	std::scoped_lock l(lock_);
  LOG_TRACE();
  time_step_->Build();
  initialisation_phase_->Build(model_);
  process_->Build();  // To handle BH Recruitment having ssb_offset available

  addressable_input_loader_->Build();
  additional_prior_->Build();
  ageing_error_->Build();
  assert_->Build();
  catchability_->Build();
  derived_quantity_->Build();
  likelihood_->Build();

  observation_->Build();

  // age-length and length-weight needs to be down stream of observations and processes
  // so they can tell the age-length class which years to build age-length matricies for.
  age_length_->Build();
  age_weight_->Build();
  length_weight_->Build();

  penalty_->Build();
  profile_->Build();
  project_->Build(model_);
  selectivity_->Build();
  simulate_->Build();
  time_varying_->Build();

  LOG_FINE() << "Building estimates and transformations...";
  estimate_->Build(model_);
  addressable_transformation_->Build();
  LOG_FINE() << "Building estimates and transformations...Done";
  
  if (mcmc_ && (run_mode == RunMode::kMCMC || run_mode == RunMode::kTesting))
    mcmc_->Build();
  if (run_mode == RunMode::kEstimation || run_mode == RunMode::kMCMC || run_mode == RunMode::kTesting)
    minimiser_->Build();

  if (report_)
    report_->Build(model_->pointer());
  LOG_TRACE();
}

/**
 * @brief Verify all of our managers. Verify is used to validate
 * business rules after the build has completed. At this point
 * the entire model is ready for execution. We can check the entire
 * loaded configuration for some basic things (e.g. is there ageing)
 */
void Managers::Verify(shared_ptr<Model> model) {
  std::scoped_lock l(lock_);
  LOG_TRACE();
  addressable_input_loader_->Verify(model);
  addressable_transformation_->Verify(model);
  age_length_->Verify(model);
  age_weight_->Verify(model);
  length_weight_->Verify(model);
  selectivity_->Verify(model);
  additional_prior_->Verify(model);
  ageing_error_->Verify(model);
  assert_->Verify(model);
  catchability_->Verify(model);
  derived_quantity_->Verify(model);
  estimate_->Verify(model);
  initialisation_phase_->Verify(model);
  likelihood_->Verify(model);
  observation_->Verify(model);
  penalty_->Verify(model);
  process_->Verify(model);
  profile_->Verify(model);
  project_->Verify(model);
  simulate_->Verify(model);
  time_step_->Verify(model);
  time_varying_->Verify(model);
}

/**
 * @brief
 *
 */
void Managers::Reset() {
  std::scoped_lock l(lock_);
  LOG_TRACE();
  addressable_transformation_->Reset();  // needs to be up top, asmay change values down the list
  age_length_->Reset();
  age_weight_->Reset();
  length_weight_->Reset();
  selectivity_->Reset();
  additional_prior_->Reset();
  ageing_error_->Reset();
  assert_->Reset();
  catchability_->Reset();
  derived_quantity_->Reset();
  estimate_->Reset();
  initialisation_phase_->Reset();
  likelihood_->Reset();
  observation_->Reset();
  penalty_->Reset();
  process_->Reset();
  profile_->Reset();
  project_->Reset();
  //  report_->Reset();
  simulate_->Reset();
  time_step_->Reset();
  time_varying_->Reset();
}

} /* namespace niwa */
