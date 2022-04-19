/**
 * @file CompoundAll.h
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
#ifndef COMPOUND_ALL_H_
#define COMPOUND_ALL_H_

// Headers
#include "../../Selectivities/Selectivity.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Class definition
 */
class CompoundAll : public niwa::Selectivity {
public:
  // Methods
  explicit CompoundAll(shared_ptr<Model> model);
  virtual ~CompoundAll() = default;
  void DoValidate() override final;

  Double get_value(Double value)  override final; 
  Double get_value(unsigned value)  override final; 

private:
  // Members
  Double a50_;
  Double a_to95_;
  Double amin_;
  Double alpha_;

};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* COMPOUND_ALL_H_ */
