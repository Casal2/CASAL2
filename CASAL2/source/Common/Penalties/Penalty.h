/**
 * @file Penalty.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * A penalty is an object that when trigger applies a score
 * to the objective function. Certain processes and objects
 * will trigger a penalty when extreme conditions are met
 * that are not encouraging the minimiser to find the minimum.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PENALTY_H_
#define PENALTY_H_

// Headers
#include "Common/BaseClasses/Object.h"
#include "Common/Utilities/Types.h"

// Namespaces
namespace niwa {
class Model;
using niwa::utilities::Double;

/**
 * Class definition
 */
class Penalty : public niwa::base::Object {
public:
  // Methods
  Penalty() = delete;
  explicit Penalty(Model* model);
  virtual                     ~Penalty() = default;
  void                        Validate();
  void                        Build() { DoBuild(); };
  void                        Reset() {};
  virtual Double              GetScore() = 0;
  virtual void                Trigger(const string& source_label, Double value_1, Double value_2) { };

  // accessors
  bool                        has_score() const { return has_score_; }

protected:
  // methods
  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;

  // members
  Model*                      model_ = nullptr;
  bool                        has_score_ = true;
};
} /* namespace niwa */
#endif /* PENALTY_H_ */
