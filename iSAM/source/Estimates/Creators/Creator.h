/**
 * @file Creator.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 22/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This object is the object that has a 1:1 link to the @estimate block in the configuration file.
 * From this object we'll create 1-N estimates.
 */
#ifndef ESTIMATES_CREATOR_H_
#define ESTIMATES_CREATOR_H_

// headers
#include "BaseClasses/Object.h"
#include "Estimates/Estimate.h"

// namespaces
namespace isam {
namespace estimates {

class Manager;

/**
 *
 */
class Creator : public isam::base::Object {
public:
  // methods
  Creator();
  virtual                     ~Creator() = default;
  void                        CreateEstimates();

protected:
  // methods
  isam::EstimatePtr           CreateEstimate(string label, unsigned index, Double* target);
  void                        CopyParameters(isam::EstimatePtr estimate, unsigned index);
  virtual void                DoCopyParameters(isam::EstimatePtr estimate, unsigned index) = 0;

  // members
  string                      parameter_;
  vector<Double>              lower_bounds_;
  vector<Double>              upper_bounds_;
  string                      prior_label_;
  vector<string>              same_labels_;
  string                      estimation_phase_;
  string                      mcmc_;
  bool                        update_label_;
  bool                        update_parameter_;
  vector<string>              transformation_details_;
};

// typedef
typedef boost::shared_ptr<Creator> CreatorPtr;

} /* namespace estimates */
} /* namespace isam */

#endif /* ESTIMATES_CREATOR_H_ */
