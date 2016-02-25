/**
 * @file SquareRoot.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Dec 7, 2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SQUAREROOT_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SQUAREROOT_H_

// headers
#include "EstimateTransformations/EstimateTransformation.h"

// namespaces
namespace niwa {
class Estimate;
namespace estimatetransformations {

// classes
class SquareRoot : public EstimateTransformation {
public:
  // methods
  SquareRoot() = delete;
  explicit SquareRoot(Model* model);
  virtual ~SquareRoot() = default;
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
  string                      estimate_label_ = "";
  Estimate*                   estimate_;
};

} /* namespace estimatetransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SQUAREROOT_H_ */
