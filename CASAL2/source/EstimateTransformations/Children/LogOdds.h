/**
 * @file LogOdds.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 8, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_LOGODDS_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_LOGODDS_H_

// headers
#include "EstimateTransformations/EstimateTransformation.h"

// namespaces
namespace niwa {
class Estimate;
namespace estimatetransformations {

// classes
class LogOdds : public EstimateTransformation {
public:
  LogOdds() = delete;
  explicit LogOdds(Model* model);
  virtual ~LogOdds() = default;
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
  Estimate*                   estimate_ = nullptr;
};

} /* namespace estimatetransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_LOGODDS_H_ */
