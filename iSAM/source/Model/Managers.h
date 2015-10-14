/**
 * @file Managers.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 11/08/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class holds accessors for the managers in the application. I've gone down
 * this path because I want to be able to easily mock them for use in the unit
 * test suite.
 */
#ifndef SOURCE_MODEL_MANAGERS_H_
#define SOURCE_MODEL_MANAGERS_H_

// namespaces
namespace niwa {

// forward decs
namespace additionalpriors { class Manager; }
namespace ageingerrors { class Manager; }
namespace agelengths { class Manager; }
namespace asserts { class Manager; }
namespace catchabilities { class Manager; }
namespace derivedquantities { class Manager; }
namespace estimates { class Manager; }
namespace initialisationphases { class Manager; }
namespace lengthweights { class Manager; }
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

class Estimables;
class Model;

// classes
class Managers {
  friend class Model;
public:
  // accessors
  virtual additionalpriors::Manager*      additional_prior() { return additional_prior_; }
  virtual ageingerrors::Manager*          ageing_error() { return ageing_error_; }
  virtual agelengths::Manager*            age_length() { return age_length_; }
  virtual asserts::Manager*               assertx() { return assert_; }
  virtual catchabilities::Manager*        catchability() { return catchability_; }
  virtual derivedquantities::Manager*     derived_quantity() { return derived_quantity_; }
  virtual Estimables*                     estimables() { return estimables_; }
  virtual estimates::Manager*             estimate() { return estimate_; }
  virtual initialisationphases::Manager*  initialisation_phase() { return initialisation_phase_; }
  virtual lengthweights::Manager*         length_weight() { return length_weight_; }
  virtual mcmcs::Manager*                 mcmc() { return mcmc_; }
  virtual minimisers::Manager*            minimiser() { return minimiser_; }
  virtual observations::Manager*          observation() { return observation_; }
  virtual penalties::Manager*             penalty() { return penalty_; }
  virtual processes::Manager*             process() { return process_; }
  virtual profiles::Manager*              profile() { return profile_; }
  virtual projects::Manager*              project() { return project_; }
  virtual reports::Manager*               report() { return report_; }
  virtual selectivities::Manager*         selectivity() { return selectivity_; }
  virtual simulates::Manager*             simulate() { return simulate_; }
  virtual timesteps::Manager*             time_step() { return time_step_; }
  virtual timevarying::Manager*           time_varying() { return time_varying_; }

protected:
  // methods
  Managers(Model* model);
  virtual                     ~Managers();
  void                        Validate();
  void                        Build();
  void                        Reset();

  // members
  Model*                              model_;
  additionalpriors::Manager*          additional_prior_;
  ageingerrors::Manager*              ageing_error_;
  agelengths::Manager*                age_length_;
  asserts::Manager*                   assert_;
  catchabilities::Manager*            catchability_;
  derivedquantities::Manager*         derived_quantity_;
  Estimables*                         estimables_; // TODO: Move to Model
  estimates::Manager*                 estimate_;
  initialisationphases::Manager*      initialisation_phase_;
  lengthweights::Manager*             length_weight_;
  mcmcs::Manager*                     mcmc_;
  minimisers::Manager*                minimiser_;
  observations::Manager*              observation_;
  penalties::Manager*                 penalty_;
  processes::Manager*                 process_;
  profiles::Manager*                  profile_;
  projects::Manager*                  project_;
  reports::Manager*                   report_;
  selectivities::Manager*             selectivity_;
  simulates::Manager*                 simulate_;
  timesteps::Manager*                 time_step_;
  timevarying::Manager*               time_varying_;
};

} /* namespace niwa */

#endif /* SOURCE_MODEL_MANAGERS_H_ */
