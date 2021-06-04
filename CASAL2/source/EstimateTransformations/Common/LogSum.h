/**
 * @file LogSum.h
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date Jul 8, 2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 * @section
 * This transformation takes two parameters (x1 and x2) and estimates y1 = log(x1 + x2) and p1 where x1 = exp(y1)*p1 and x2 = exp(y1)* (1 - p1)
 * Used in the hoki stock assessment to when they have two stock B0 estimates for some reason they choice to estimate prop and log B0
 */
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_LOGSUM_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_LOGSUM_H_

// headers
#include "../../EstimateTransformations/EstimateTransformation.h"

// namespaces
namespace niwa {
namespace estimatetransformations {

// classes
class LogSum : public EstimateTransformation {
public:
  LogSum() = delete;
  explicit LogSum(shared_ptr<Model> model);
  virtual ~LogSum() = default;
  void             TransformForObjectiveFunction() override final;
  void             RestoreFromObjectiveFunction() override final;
  std::set<string> GetTargetEstimates() override final;
  Double           GetScore() override final { return jacobian_; };

protected:
  // methods
  void DoValidate() override final;
  void DoBuild() override final;
  void DoTransform() override final;
  void DoRestore() override final;

private:
  // members
  string    second_estimate_label_ = "";
  Estimate* second_estimate_       = nullptr;
  Double    xt_;
};

} /* namespace estimatetransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_LOGSUM_H_ */
