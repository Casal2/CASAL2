/*
 * EstimateTransformation.h
 *
 *  Created on: Dec 7, 2015
 *      Author: Zaita
 */

#ifndef SOURCE_ESTIMATETRANSFORMATIONS_ESTIMATETRANSFORMATION_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_ESTIMATETRANSFORMATION_H_

// headers
#include <set>

#include "BaseClasses/Object.h"

// namespaces
namespace niwa {
using std::set;
class Model;

// classes
class EstimateTransformation : public niwa::base::Object {
public:
  // methods
  EstimateTransformation() = delete;
  explicit EstimateTransformation(Model* model);
  virtual                     ~EstimateTransformation() = default;
  void                        Validate();
  void                        Build() { DoBuild();};
  void                        Reset() { };
  // pure virtual
  virtual void                Transform() = 0;
  virtual void                Restore() = 0;
  virtual std::set<string>    GetTargetEstimates() = 0;
  virtual Double              GetScore() = 0;

protected:
  // methods
  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;

  // members
  Model*                      model_ = nullptr;
  Double                      lower_bound_ = 0.0;
  Double                      upper_bound_ = 0.0;
  Double                      original_lower_bound_ = 0.0;
  Double                      original_upper_bound_ = 0.0;
  Double                      original_value_ = 0.0;
  Double                      jacobian_ = 0.0;
};

} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_ESTIMATETRANSFORMATION_H_ */
