/**
 * @file LengthWeight.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef LENGTHWEIGHT_H_
#define LENGTHWEIGHT_H_

// headers
#include "../AgeLengths/AgeLength.h"  // Check if we need this now that we have moved DIstribution out of AgeLenght
#include "../BaseClasses/Object.h"
#include "../Model/Model.h"
#include "../Utilities/Distribution.h"

// namespaces
namespace niwa {
/**
 * class definition
 */
class LengthWeight : public niwa::base::Object {
public:
  // methods
  LengthWeight() = delete;
  explicit LengthWeight(shared_ptr<Model> model);
  virtual ~LengthWeight(){};
  void Validate();
  void Build() { DoBuild(); };
  void Verify(shared_ptr<Model> model){};
  void Reset() { DoReset(); };

  virtual void DoValidate() = 0;
  virtual void DoBuild()    = 0;
  virtual void DoReset()    = 0;

  // accessors
  virtual Double mean_weight(Double size, Distribution distribution, Double cv) const = 0;
  virtual Double mean_weight(Double size) const = 0;  // overloader for length based models
  virtual string weight_units() const                                                 = 0;

  // members
  shared_ptr<Model> model_ = nullptr;
};
} /* namespace niwa */
#endif /* LENGTHWEIGHT_H_ */
