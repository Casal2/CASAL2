/**
 * @file Catchability.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/03/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * Catchabilities are used to scale up or down observations with expectations
 *
 */
#ifndef CATCHABILITY_H_
#define CATCHABILITY_H_

// Headers
#include "../BaseClasses/Object.h"
#include "../Model/Model.h"

// Namespaces
namespace niwa {

// classes
class Catchability : public niwa::base::Object {
public:
  // Methods
  Catchability() = delete;
  explicit Catchability(shared_ptr<Model> model);
  virtual ~Catchability() = default;
  void         Validate();
  void         Build() { DoBuild(); };
  void         Verify(shared_ptr<Model> model) {DoVerify(model);};
  void         Reset(){};
  void         SaveObservationLabel(string observation_label) {observation_labels_.push_back(observation_label);};
  virtual void DoValidate() = 0;
  virtual void DoBuild()    = 0;
  virtual void DoVerify(shared_ptr<Model> model) {};

  // Accessors
  Double q() const { return q_; };

protected:
  // members
  shared_ptr<Model> model_ = nullptr;
  Double            q_     = 0.0;
  vector<string>    observation_labels_;
};

} /* namespace niwa */
#endif /* CATCHABILITY_H_ */
