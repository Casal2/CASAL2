/*
 * Manager.h
 *
 *  Created on: Dec 7, 2015
 *      Author: Zaita
 */

#ifndef SOURCE_ESTIMABLETRANSFORMATIONS_MANAGER_H_
#define SOURCE_ESTIMABLETRANSFORMATIONS_MANAGER_H_

// Headers
#include "../BaseClasses/Manager.h"
#include "EstimableTransformation.h"
#include "../Model/Managers.h"

// Namespaces
namespace niwa {
namespace estimabletransformations {

/**
 * Class Definition
 */
class Manager : public niwa::base::Manager<niwa::estimabletransformations::Manager, niwa::EstimableTransformation> {
  friend class niwa::base::Manager<niwa::estimabletransformations::Manager, niwa::EstimableTransformation>;
  friend class niwa::Managers;

public:
  virtual ~Manager() = default;
  void Validate() override final;
  EstimableTransformation*         GetEstimableTransformation(const string& label);

protected:
  // methods
  Manager() = default;
};

} /* namespace estimabletransformations */
} /* namespace niwa */
#endif /* SOURCE_ESTIMABLETRANSFORMATIONS_MANAGER_H_ */
