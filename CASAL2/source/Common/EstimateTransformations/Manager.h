/*
 * Manager.h
 *
 *  Created on: Dec 7, 2015
 *      Author: Zaita
 */

#ifndef SOURCE_ESTIMATETRANSFORMATIONS_MANAGER_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_MANAGER_H_

// Headers
#include "Common/BaseClasses/Manager.h"
#include "Common/EstimateTransformations/EstimateTransformation.h"
#include "Common/Model/Managers.h"

// Namespaces
namespace niwa {
namespace estimatetransformations {

/**
 * Class Definition
 */
class Manager : public niwa::base::Manager<niwa::estimatetransformations::Manager, niwa::EstimateTransformation> {
  friend class niwa::base::Manager<niwa::estimatetransformations::Manager, niwa::EstimateTransformation>;
  friend class niwa::Managers;
public:
  virtual                     ~Manager() = default;
  void                        Validate() override final;
  void                        TransformEstimates();
  void                        RestoreEstimates();
  void                        TransformEstimatesForObjectiveFunction();
  void                        RestoreEstimatesFromObjectiveFunction();

protected:
  // methods
  Manager() = default;
};

} /* namespace estimatetransformations */
} /* namespace niwa */
#endif /* SOURCE_ESTIMATETRANSFORMATIONS_MANAGER_H_ */
