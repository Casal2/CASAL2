/**
 * @file Catchability.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
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
  void         Verify(shared_ptr<Model> model){};
  void         Reset(){};
  virtual void DoValidate() = 0;
  virtual void DoBuild()    = 0;

  // Accessors
  Double q() const { return q_; };

protected:
  // members
  shared_ptr<Model> model_ = nullptr;
  Double            q_     = 0.0;
};

} /* namespace niwa */
#endif /* CATCHABILITY_H_ */
