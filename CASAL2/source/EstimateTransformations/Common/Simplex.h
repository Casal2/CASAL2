/**
 * @file Simplex.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 8, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Follows unit simplex theory, I used chapter 55.6 in the STAN reference map for the jacobian calculation.
 */
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SIMPLEX_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SIMPLEX_H_

// headers
#include "../../EstimateTransformations/EstimateTransformation.h"

// namespaces
namespace niwa {
class Estimate;
namespace estimatetransformations {

/**
 *
 */
class Simplex : public EstimateTransformation {
public:
  Simplex() = delete;
  explicit                    Simplex(shared_ptr<Model> model);
  virtual                     ~Simplex() = default;
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
  double                      lower_bound_ = 1.0e-10;
  double                      upper_bound_ = 1.0e10;

  vector<Estimate*>           estimates_;
  vector<Estimate*>           simplex_estimates_;
  Double                      sub_total_ = 0.0;
  Double                      total_ = 0.0;
  unsigned                    length_ = 0;
};

} /* namespace estimatetransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SIMPLEX_H_ */
