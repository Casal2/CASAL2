/**
 * @file LengthWeight.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef LENGTHWEIGHT_H_
#define LENGTHWEIGHT_H_

// headers
#include "BaseClasses/Object.h"
#include "Model/Model.h"
#include "AgeLengths/AgeLength.h" // Check if we need this now that we have moved DIstribution out of AgeLenght
#include "Utilities/Distribution.h"

// namespaces
namespace niwa {
/**
 * class definition
 */
class LengthWeight : public niwa::base::Object {
public:
  // methods
  LengthWeight() = delete;
  explicit                          LengthWeight(Model* model);
  virtual                           ~LengthWeight() { };
  void                              Validate();
  void                              Build() { DoBuild(); };
  void                              Reset() { DoReset(); };

  virtual void                      DoValidate() = 0;
  virtual void                      DoBuild() = 0;
  virtual void                      DoReset() = 0;

  // accessors
  virtual Double                    mean_weight(Double size, Distribution distribution, Double cv) const = 0;

  // members
  Model*                            model_ = nullptr;

};
} /* namespace niwa */
#endif /* LENGTHWEIGHT_H_ */
