/**
 * @file CompoundLeft.h
 * @author  C.Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef COMPOUND_LEFT_H_
#define COMPOUND_LEFT_H_

// Headers
#include "../../Selectivities/Selectivity.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Class definition
 */
class CompoundLeft : public niwa::Selectivity {
public:
  // Methods
  explicit CompoundLeft(shared_ptr<Model> model);
  virtual ~CompoundLeft() = default;
  void DoValidate() override final;

  Double get_value(Double value)  override final; 
  Double get_value(unsigned value)  override final; 

private:
  // Members
  Double a50_;
  Double a_to95_;
  Double amin_;
  Double leftmu_;
  Double sd_;
  Double alpha_;

};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* COMPOUND_LEFT_H_ */
