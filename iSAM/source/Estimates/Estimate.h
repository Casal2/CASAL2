/**
 * @file Estimate.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class represents a currently loaded estimate. Each
 * estimate is tied to a specific registered estimable in the
 * system. There is a 1:1 relationship between estimate and estimable
 * (excluding when the 'same' keyword is used).
 *
 * This object is not constructed directly from a configuration file
 * block, instead the isam::estimates::Info is used to create this.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef ESTIMATE_H_
#define ESTIMATE_H_

// Headers
#include "BaseClasses/Object.h"
#include "Priors/Prior.h"
#include "Utilities/Types.h"

// Namespaces
namespace isam {

using isam::utilities::Double;

/**
 * Class Definition
 */
class Estimate : public isam::base::Object {
public:
  // Methods
  Estimate();
  virtual                     ~Estimate();
  void                        Validate();
  void                        Build();
  void                        Reset() { };
  Double                      GetPriorScore();

  // Accessors
  void                        set_target(Double* new_target) { target_ = new_target; };
  string                      parameter() const { return parameter_; }
  Double                      lower_bound() const { return lower_bound_; }
  Double                      upper_bound() const { return upper_bound_; }
  bool                        enabled() const { return enabled_; }
  void                        set_enabled(bool new_value) { enabled_ = new_value; }
  Double                      value() { return *target_; }
  void                        set_value(Double new_value) { *target_ = new_value; }
  PriorPtr                    prior() { return prior_; }

private:
  // Members
  Double*                     target_;
  string                      parameter_;
  Double                      lower_bound_;
  Double                      upper_bound_;
  bool                        mcmc_fixed_;
  string                      prior_label_;
  unsigned                    estimation_phase_;
  vector<string>              same_labels;
  vector<Double*>             sames_;
  bool                        enabled_ = true;
  PriorPtr                    prior_;
};

typedef boost::shared_ptr<isam::Estimate> EstimatePtr;

} /* namespace isam */
#endif /* ESTIMATE_H_ */
