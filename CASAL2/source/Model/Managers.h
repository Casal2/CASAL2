/**
 * @file Managers.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 11/08/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class holds accessors for the managers in the application. I've gone down
 * this path because I want to be able to easily mock them for use in the unit
 * test suite.
 */
#ifndef SOURCE_MODEL_MANAGERS_H_
#define SOURCE_MODEL_MANAGERS_H_

#include <memory>
#include <mutex>

#include "../Logging/Logging.h"

// macro to check return pointers
#define CHECK_AND_RETURN(x)                                              \
  if (x == nullptr)                                                      \
    LOG_CODE_ERROR() << "x == nullptr: " << __FILE__ << ":" << __LINE__; \
  return x;

// namespaces
namespace niwa {

// forward decs
// clang-format off
namespace additionalpriors { class Manager; }
namespace ageingerrors { class Manager;}
namespace agelengths { class Manager; }
namespace ageweights { class Manager; }
namespace asserts { class Manager; }
namespace catchabilities { class Manager; }
namespace derivedquantities { class Manager; }
namespace estimates { class Manager; }
namespace estimatetransformations { class Manager;}
namespace initialisationphases { class Manager; }
namespace lengthweights { class Manager; }
namespace likelihoods { class Manager; }
namespace mcmcs { class Manager; }
namespace minimisers { class Manager; }
namespace observations { class Manager; }
namespace penalties { class Manager; }
namespace processes { class Manager; }
namespace profiles { class Manager; }
namespace projects { class Manager; }
namespace reports { class Manager; }
namespace selectivities { class Manager; }
namespace simulates { class Manager; }
namespace timesteps { class Manager; }
namespace timevarying { class Manager; }
// clang-format on

class Estimables;
class Model;
using std::shared_ptr;

// classes
class Managers {
  friend class Model;
  friend class MockManagers;

public:
  Managers() = delete;
  virtual ~Managers();
  void create_test_managers();

  // accessors
  virtual additionalpriors::Manager*        additional_prior() { CHECK_AND_RETURN(additional_prior_); }
  virtual ageingerrors::Manager*            ageing_error() { CHECK_AND_RETURN(ageing_error_); }
  virtual agelengths::Manager*              age_length() { CHECK_AND_RETURN(age_length_); }
  virtual ageweights::Manager*              age_weight() { CHECK_AND_RETURN(age_weight_); }
  virtual asserts::Manager*                 assertx() { CHECK_AND_RETURN(assert_); }
  virtual catchabilities::Manager*          catchability() { CHECK_AND_RETURN(catchability_); }
  virtual derivedquantities::Manager*       derived_quantity() { CHECK_AND_RETURN(derived_quantity_); }
  virtual Estimables*                       estimables() { CHECK_AND_RETURN(estimables_); }
  virtual estimates::Manager*               estimate() { CHECK_AND_RETURN(estimate_); }
  virtual estimatetransformations::Manager* estimate_transformation() { CHECK_AND_RETURN(estimate_transformation_); }
  virtual initialisationphases::Manager*    initialisation_phase() { CHECK_AND_RETURN(initialisation_phase_); }
  virtual lengthweights::Manager*           length_weight() { CHECK_AND_RETURN(length_weight_); }
  virtual likelihoods::Manager*             likelihood() { CHECK_AND_RETURN(likelihood_); }
  virtual shared_ptr<mcmcs::Manager>        mcmc();
  virtual shared_ptr<minimisers::Manager>   minimiser();
  virtual observations::Manager*            observation() { CHECK_AND_RETURN(observation_); }
  virtual penalties::Manager*               penalty() { CHECK_AND_RETURN(penalty_); }
  virtual processes::Manager*               process() { CHECK_AND_RETURN(process_); }
  virtual profiles::Manager*                profile() { CHECK_AND_RETURN(profile_); }
  virtual projects::Manager*                project() { CHECK_AND_RETURN(project_); }
  virtual shared_ptr<reports::Manager>      report();
  virtual selectivities::Manager*           selectivity() { CHECK_AND_RETURN(selectivity_); }
  virtual simulates::Manager*               simulate() { CHECK_AND_RETURN(simulate_); }
  virtual timesteps::Manager*               time_step() { CHECK_AND_RETURN(time_step_); }
  virtual timevarying::Manager*             time_varying() { CHECK_AND_RETURN(time_varying_); }

  void set_minimiser(shared_ptr<minimisers::Manager> manager) { minimiser_ = manager; }
  void set_reports(shared_ptr<reports::Manager> manager) { report_ = manager; }
  void set_mcmc(shared_ptr<mcmcs::Manager> manager) { mcmc_ = manager; }

protected:
  // methods
  Managers(shared_ptr<Model> model);
  void Validate();
  void Build();
  void Reset();

  // members
  shared_ptr<Model>                 model_;
  additionalpriors::Manager*        additional_prior_        = nullptr;
  ageingerrors::Manager*            ageing_error_            = nullptr;
  agelengths::Manager*              age_length_              = nullptr;
  ageweights::Manager*              age_weight_              = nullptr;
  asserts::Manager*                 assert_                  = nullptr;
  catchabilities::Manager*          catchability_            = nullptr;
  derivedquantities::Manager*       derived_quantity_        = nullptr;
  Estimables*                       estimables_              = nullptr;  // TODO: Move to Model
  estimates::Manager*               estimate_                = nullptr;
  estimatetransformations::Manager* estimate_transformation_ = nullptr;
  initialisationphases::Manager*    initialisation_phase_    = nullptr;
  lengthweights::Manager*           length_weight_           = nullptr;
  likelihoods::Manager*             likelihood_              = nullptr;
  shared_ptr<mcmcs::Manager>        mcmc_;
  shared_ptr<minimisers::Manager>   minimiser_;
  observations::Manager*            observation_ = nullptr;
  penalties::Manager*               penalty_     = nullptr;
  processes::Manager*               process_     = nullptr;
  profiles::Manager*                profile_     = nullptr;
  projects::Manager*                project_     = nullptr;
  shared_ptr<reports::Manager>      report_;
  selectivities::Manager*           selectivity_  = nullptr;
  simulates::Manager*               simulate_     = nullptr;
  timesteps::Manager*               time_step_    = nullptr;
  timevarying::Manager*             time_varying_ = nullptr;
  static std::mutex                 lock_;
};

} /* namespace niwa */

#endif /* SOURCE_MODEL_MANAGERS_H_ */
