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
class Estimate;
namespace estimatetransformations {

// classes
class AverageDifference : public EstimateTransformation {
public:
  AverageDifference() = delete;
  explicit AverageDifference(Model* model);
  virtual ~AverageDifference() = default;
  void                        Transform() override final;
  void                        Restore() override final;
  std::set<string>            GetTargetEstimates() override final;
  Double                      GetScore() override final {return jacobian_;};

protected:
  // methods
  void                        DoValidate() override final;
  void                        DoBuild() override final;

private:
  // members
  string                      first_estimate_label_ = "";
  string                      second_estimate_label_ = "";
  bool                        first_value_bigger_ = false;
  Estimate*                   first_estimate_ = nullptr;
  Estimate*                   second_estimate_ = nullptr;

};

} /* namespace estimatetransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_AVERAGEDIFFERENCE_H_ */
