/**
 * @file Creator.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 22/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This object is the object that has a 1:1 link to the @estimate block in the configuration file.
 * From this object we'll create 1-N estimates.
 */
#ifndef ESTIMATES_CREATOR_H_
#define ESTIMATES_CREATOR_H_

// headers
#include "../../BaseClasses/Object.h"
#include "../../Estimates/Estimate.h"

// namespaces
namespace niwa {
class Model;

namespace estimates {
class Manager;

/**
 *
 */
class Creator : public niwa::base::Object {
public:
  // methods
  Creator() = delete;
  explicit Creator(shared_ptr<Model> model);
  virtual ~Creator() = default;
  void CreateEstimates();
  void Reset() override final{};

  // accessors
  string parameter() const { return parameter_; }

protected:
  // methods
  niwa::Estimate* CreateEstimate(string parameter, unsigned index, Double* target);
  void            HandleSameParameter();
  void            CopyParameters(niwa::Estimate* estimate, unsigned index);
  virtual void    DoCopyParameters(niwa::Estimate* estimate, unsigned index) = 0;

  // members
  shared_ptr<Model>       model_ = nullptr;
  string                  parameter_;
  vector<double>          lower_bounds_;
  vector<double>          upper_bounds_;
  string                  prior_label_;
  vector<string>          same_labels_;
  string                  estimation_phase_;
  string                  mcmc_;
  bool                    transform_for_objective_function_;
  vector<string>          transformation_details_;
  vector<bool>            transform_with_jacobian_;
  vector<niwa::Estimate*> estimates_;
};

} /* namespace estimates */
} /* namespace niwa */

#endif /* ESTIMATES_CREATOR_H_ */
