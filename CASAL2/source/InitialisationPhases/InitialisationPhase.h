/**
 * @file InitialisationPhase.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef INITIALISATIONPHASE_H_
#define INITIALISATIONPHASE_H_

// Headers
#include "../BaseClasses/Object.h"

namespace niwa {
class Model;

/**
 * Class Defintiion
 */
class InitialisationPhase : public niwa::base::Object {
public:
  // Methods
  InitialisationPhase() = delete;
  explicit InitialisationPhase(shared_ptr<Model> model);
  virtual ~InitialisationPhase() = default;
  void         Validate();
  void         Build();
  void         Verify(shared_ptr<Model> model){};
  void         Reset(){};
  void         Execute();

protected:
  // methods
  virtual void DoValidate() = 0;
  virtual void DoBuild()    = 0;
  virtual void DoExecute()    = 0;

  // members
  shared_ptr<Model> model_ = nullptr;
};
} /* namespace niwa */
#endif /* INITIALISATIONPHASE_H_ */
