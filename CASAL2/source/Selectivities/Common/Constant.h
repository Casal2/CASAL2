/**
 * @file Constant.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 11/01/2013
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
#ifndef CONSTANT_H_
#define CONSTANT_H_

// Headers
#include "../../Selectivities/Selectivity.h"

// Namespacaes
namespace niwa {
namespace selectivities {

/**
 * Class Definition
 */
class Constant : public niwa::Selectivity {
public:
  // Methods
  explicit Constant(shared_ptr<Model> model);
  virtual ~Constant() = default;
  void   DoValidate() override final;
  Double GetAgeResult(unsigned age, AgeLength* age_length) override;
  Double GetLengthResult(unsigned length_bin) override final;

  Double get_value(Double value) override final;
  Double get_value(unsigned value) override final;

private:
  // Members
  Double c_;
  Double beta_;
};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* CONSTANT_H_ */
