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
 * block, instead the niwa::estimates::Info is used to create this.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef ESTIMATE_H_
#define ESTIMATE_H_

// Headers
#include "BaseClasses/Object.h"
#include "Estimates/Info.h"
#include "Estimates/Transformations/Transformation.h"
#include "Utilities/Types.h"

// Namespaces
namespace niwa {

using niwa::utilities::Double;

/**
 * Class Definition
 */
class Estimate : public niwa::base::Object {
public:
  // Methods
  Estimate();
  virtual                     ~Estimate() = default;
  void                        Validate();
  void                        Build() { };
  void                        Reset() { };
  Double                      GetTransformedValue();
  void                        SetTransformedValue(Double minimiser_value);

  // pure methods
  virtual void                DoValidate() = 0;
  virtual Double              GetScore() = 0;

  // Accessors
  void                        set_parent_info(estimates::EstimateInfoPtr parent_info) { parent_info_ = parent_info; }
  estimates::EstimateInfoPtr  parent_info() const { return parent_info_; }
  void                        set_target(Double* new_target) { target_ = new_target; };
  string                      parameter() const { return parameter_; }
  Double                      lower_bound() const { return lower_bound_; }
  Double                      upper_bound() const { return upper_bound_; }
  bool                        enabled() const { return enabled_; }
  void                        set_enabled(bool new_value) { enabled_ = new_value; }
  Double                      value() { return *target_; }
  void                        set_value(Double new_value) { *target_ = new_value; }
  bool                        mcmc_fixed() const { return mcmc_fixed_; }

protected:
  // Members
  Double*                     target_ = 0;
  string                      parameter_;
  Double                      lower_bound_;
  Double                      upper_bound_;
  bool                        mcmc_fixed_;
  string                      prior_label_;
  unsigned                    estimation_phase_;
  vector<string>              same_labels;
  vector<Double*>             sames_;
  bool                        enabled_ = true;
  estimates::EstimateInfoPtr  parent_info_;
  vector<string>              transformation_details_;
  estimates::TransformationPtr  transformation_;
};

typedef boost::shared_ptr<niwa::Estimate> EstimatePtr;

} /* namespace niwa */
#endif /* ESTIMATE_H_ */
