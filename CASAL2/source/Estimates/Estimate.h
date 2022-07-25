/**
 * @file Estimate.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class represents a currently loaded estimate. Each
 * estimate is tied to a specific registered addressable in the
 * system. There is a 1:1 relationship between estimate and addressable
 * (excluding when the 'same' keyword is used).
 *
 * This object is not constructed directly from a configuration file
 * block, instead the niwa::estimates::Info is used to create this.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef ESTIMATE_H_
#define ESTIMATE_H_

// Headers
#include "../BaseClasses/Object.h"
#include "../Utilities/Types.h"

// Namespaces
namespace niwa {
class Model;
using niwa::utilities::Double;

/**
 * Class Definition
 */
class Estimate : public niwa::base::Object {
public:
  // Methods
  Estimate() = delete;
  explicit Estimate(shared_ptr<Model> model);
  virtual ~Estimate() = default;
  void Validate();
  void Build();
  void Verify(shared_ptr<Model> model){};
  void Reset();
  void AddSame(const string& label, Double* target);

  // pure methods
  virtual void   DoValidate() = 0;
  virtual Double GetScore()   = 0;
  virtual vector<Double>   GetPriorValues() = 0;
  virtual vector<string>   GetPriorLabels() = 0;
  // Accessors
  void                   set_target(Double* new_target) { target_ = new_target; };
  void                   set_creator_parameter(const string& parameter) { creator_parameter_ = parameter; }
  string                 creator_parameter() const { return creator_parameter_; }
  string                 parameter() const { return parameter_; }
  Double                 lower_bound() const { return lower_bound_; }
  void                   set_lower_bound(Double new_value) { lower_bound_ = new_value; }
  Double                 upper_bound() const { return upper_bound_; }
  void                   set_upper_bound(Double new_value) { upper_bound_ = new_value; }
  bool                   estimated() const { return estimated_; }
  bool                   estimated_in_phasing() const { return estimated_in_phasing_; }
  void                   set_estimated(bool new_value) { estimated_ = new_value; }
  void                   set_estimated_in_phasing(bool new_value) { estimated_in_phasing_ = new_value; }
  void                   set_mcmc_fixed(bool new_value) { mcmc_fixed_ = new_value; }
  Double                 value() { return *target_; }
  Double                 get_initial_value() { return initial_value_; }
  void                   set_value(Double new_value);
  void                   set_initial_value(Double new_value) {initial_value_ = new_value;}; // currently used by AddressableInputLoader if users use -i
  void                   flag_value_has_been_initialised() {value_been_initialised_ = true;};
  bool                   mcmc_fixed() const { return mcmc_fixed_; }
  void                   set_in_objective_function(bool value) { in_objective_ = value; }
  bool                   in_objective_function() const { return in_objective_; }
  const vector<string>&  same_labels() const { return same_labels_; }
  const vector<Double*>& sames() const { return sames_; }
  unsigned               phase() const { return estimation_phase_; }

protected:
  // Members
  shared_ptr<Model> model_  = nullptr;
  Double*           target_ = nullptr;
  string            parameter_;
  string            creator_parameter_;
  Double            lower_bound_;
  Double            upper_bound_;
  bool              mcmc_fixed_ = false;
  //  string                      prior_label_;
  unsigned        estimation_phase_ = 1;
  vector<string>  same_labels_;
  vector<Double*> sames_;
  bool            estimated_    = true;
  bool            estimated_in_phasing_               = true;
  bool            in_objective_ = true;
  Double          initial_value_;
  bool            value_been_initialised_ = false;
};
} /* namespace niwa */
#endif /* ESTIMATE_H_ */
