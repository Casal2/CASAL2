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
#include <boost/enable_shared_from_this.hpp>

#include "BaseClasses/Object.h"
#include "Estimates/Estimate.h"

// namespaces
namespace niwa {
namespace estimates {

class Manager;

/**
 *
 */
class Creator : public niwa::base::Object, public boost::enable_shared_from_this<Creator> {
public:
  // methods
  Creator();
  virtual                     ~Creator() = default;
  void                        CreateEstimates();

  // accessors
  string                      parameter() const { return parameter_; }

protected:
  // methods
  niwa::EstimatePtr           CreateEstimate(string parameter, unsigned index, Double* target);
  void                        CopyParameters(niwa::EstimatePtr estimate, unsigned index);
  virtual void                DoCopyParameters(niwa::EstimatePtr estimate, unsigned index) = 0;

  // members
  string                      parameter_;
  vector<Double>              lower_bounds_;
  vector<Double>              upper_bounds_;
  string                      prior_label_;
  vector<string>              same_labels_;
  string                      estimation_phase_;
  string                      mcmc_;
  vector<string>              transformation_details_;
};

// typedef
typedef boost::shared_ptr<Creator> CreatorPtr;

} /* namespace estimates */
} /* namespace niwa */

#endif /* ESTIMATES_CREATOR_H_ */
