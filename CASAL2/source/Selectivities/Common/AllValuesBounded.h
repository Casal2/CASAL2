/**
 * @file AllValuesBounded.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/01/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef ALLVALUESBOUNDED_H_
#define ALLVALUESBOUNDED_H_

// Headers
#include "../../Selectivities/Selectivity.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Class Definition
 */
class AllValuesBounded : public niwa::Selectivity {
public:
  // Methods
  explicit AllValuesBounded(shared_ptr<Model> model);
  virtual ~AllValuesBounded() = default;
  void DoValidate() override final;

  Double get_value(Double value)  override final; 
  Double get_value(unsigned value)  override final; 

private:
  // Members
  unsigned       low_;
  unsigned       high_;
  vector<Double> v_;
  unsigned       lower_length_bin_;
};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* ALLVALUESBOUNDED_H_ */
