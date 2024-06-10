/**
 * @file Penalty.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/02/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
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
#include "../BaseClasses/Object.h"
#include "../Utilities/Types.h"

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
  explicit Penalty(shared_ptr<Model> model);
  virtual ~Penalty() = default;
  void           Validate();
  void           Build() { DoBuild(); };
  void           Verify(shared_ptr<Model> model) {};
  void           Reset() { score_ = 0.0; LOG_FINE() << "reset penalty"; };
  virtual Double GetScore() { return score_; };
  virtual void   Trigger(Double value_1, Double value_2) {};

protected:
  // methods
  virtual void DoValidate() = 0;
  virtual void DoBuild()    = 0;

  // members
  shared_ptr<Model> model_     = nullptr;
  bool              has_score_ = true;
  Double            score_     = 0.0;
};
} /* namespace niwa */
#endif /* PENALTY_H_ */
