/**
 * @file SumToOne.h
 * @author C.Marsh
 * @github https://github.com/
 * @date Jan/11/2018
 * @section LICENSE
 *
 * Copyright NIWA Science ©2018 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Allows users to estimate a series K parameters that must sum to one as K-1 parameters by allowing the K parameter = 1 - sum of k-1 parameters
 */
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SUM_TO_ONE_
#define SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SUM_TO_ONE_

// headers
#include "EstimateTransformations/EstimateTransformation.h"

// namespaces
namespace niwa {
class Estimate;
namespace estimatetransformations {

/**
 *
 */

class SumToOne : public EstimateTransformation {
public:
  SumToOne() = delete;
  explicit                    SumToOne(Model* model);
  virtual                     ~SumToOne() = default;
  void                        TransformForObjectiveFunction() override final { };
  void                        RestoreFromObjectiveFunction() override final { };
  std::set<string>            GetTargetEstimates() override final;
  Double                      GetScore() override final;

protected:
  // methods
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoTransform() override final;
  void                        DoRestore() override final;

private:
  // members
  bool                        unit_ = false; // whether this is a unit vector or an average 1 vector.
  vector<Double>              unit_vector_;
  vector<Double>              zk_;
  vector<Double>              yk_;
  vector<double>              lower_bounds_;
  vector<double>              upper_bounds_;

  vector<string>              estimate_labels_;
  vector<Estimate*>           estimates_;

};

} /* namespace estimatetransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SUM_TO_ONE_ */
