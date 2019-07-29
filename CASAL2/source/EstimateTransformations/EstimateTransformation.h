/*
 * EstimateTransformation.h
 *
 *  Created on: Dec 7, 2015
 *      Author: Zaita
 */

#ifndef SOURCE_ESTIMATETRANSFORMATIONS_ESTIMATETRANSFORMATION_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_ESTIMATETRANSFORMATION_H_

// headers
#include <set>

#include "BaseClasses/Object.h"

#include "Model/Model.h"

// namespaces
namespace niwa {
using std::set;
class Estimate;
class Model;

// classes
class EstimateTransformation : public niwa::base::Object {
public:
  // methods
  EstimateTransformation() = delete;
  explicit EstimateTransformation(Model* model);
  virtual                     ~EstimateTransformation() = default;
  void                        Validate();
  void                        Build();
  void                        Reset() { };
  void                        Transform();
  void                        Restore();

  // pure virtual
  virtual void                TransformForObjectiveFunction() = 0;
  virtual void                RestoreFromObjectiveFunction() = 0;
  virtual std::set<string>    GetTargetEstimates() = 0;
  virtual Double              GetScore() = 0;

  bool                        is_simple() const { return is_simple_; }

protected:
  // methods
  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoTransform() = 0;
  virtual void                DoRestore() = 0;

  // members
  Model*                      model_ = nullptr;
  Estimate*                   estimate_ = nullptr;
  bool                        is_transformed_ = false;
  Double                      current_untransformed_value_ = 0.0;
  string                      estimate_label_;
  Double                      lower_bound_ = 0.0;
  Double                      upper_bound_ = 0.0;
  Double                      original_lower_bound_ = 0.0;
  Double                      original_upper_bound_ = 0.0;
  Double                      original_value_ = 0.0;
  Double                      jacobian_ = 0.0;
  bool                        is_simple_ = true;
  bool                        transform_with_jacobian_ = true;

};

} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_ESTIMATETRANSFORMATION_H_ */
