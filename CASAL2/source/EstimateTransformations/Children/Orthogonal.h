/**
 * @file Orthogonal.h
 * @author C.Marsh
 * @date 13/04/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Transfomration parameters x1 and x2 to estimate y1 = x1/x2 and y2 x1 * x2 see the manual for more infomration.
 */
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_ORTHOGONAL_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_ORTHOGONAL_H_

// headers
#include "EstimateTransformations/EstimateTransformation.h"

// namespaces
namespace niwa {
class Estimate;
namespace estimatetransformations {

/**
 *
 */
class Orthogonal : public EstimateTransformation {
public:
  Orthogonal() = delete;
  explicit Orthogonal(Model* model);
  virtual ~Orthogonal() = default;
  void                        Transform() override final;
  void                        Restore() override final;
  std::set<string>            GetTargetEstimates() override final;
  Double                      GetScore() override final;
protected:
  // methods
  void                        DoValidate() override final;
  void                        DoBuild() override final;

private:
  // members
  string                      first_estimate_label_ = "";
  string                      second_estimate_label_ = "";
  Estimate*                   first_estimate_ = nullptr;
  Estimate*                   second_estimate_ = nullptr;
  Double                      phi_1_;
  Double                      phi_2_;
  Double                      theta_1_;
  Double                      theta_2_;
  Double                      first_original_upper_bound_;
  Double                      first_original_lower_bound_;
  Double                      second_original_upper_bound_;
  Double                      second_original_lower_bound_;

};

} /* namespace estimatetransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_ORTHOGONAL_H_ */
