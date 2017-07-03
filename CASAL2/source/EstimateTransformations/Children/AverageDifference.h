/**
 * @file AverageDifference.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 8, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 * @section This methods takes two @estimate blocks everides the original methods with an estimate mean and difference
 *
 * << Add Description >>
 */
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_AVERAGEDIFFERENCE_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_AVERAGEDIFFERENCE_H_

// headers
#include "EstimateTransformations/EstimateTransformation.h"

// namespaces
namespace niwa {
namespace estimatetransformations {

// classes
class AverageDifference : public EstimateTransformation {
public:
  AverageDifference() = delete;
  explicit AverageDifference(Model* model);
  virtual ~AverageDifference() = default;
  void                        TransformForObjectiveFunction() override final;
  void                        RestoreFromObjectiveFunction() override final;
  std::set<string>            GetTargetEstimates() override final;
  Double                      GetScore() override final {return jacobian_;};

protected:
  // methods
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoTransform() override final;
  void                        DoRestore() override final;

private:
  // members
  string                      difference_estimate_label_ = "";
  Estimate*                   difference_estimate_ = nullptr;
  Double                      average_original_upper_bound_;
  Double                      average_original_lower_bound_;
  Double                      difference_original_lower_bound_;
  Double                      difference_original_upper_bound_;
  Double                      average_upper_bound_;
  Double                      average_lower_bound_;
  Double                      difference_lower_bound_;
  Double                      difference_upper_bound_;
  Double 											x1_;
};

} /* namespace estimatetransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_AVERAGEDIFFERENCE_H_ */
