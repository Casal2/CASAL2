/**
 * @file InverseLogistic.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef INVERSELOGISTIC_H_
#define INVERSELOGISTIC_H_

// Headers
#include "Common/Selectivities/Selectivity.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Class Definition
 */
class InverseLogistic : public niwa::Selectivity {
public:
  // Methods
  explicit InverseLogistic(Model* model);
  virtual                     ~InverseLogistic() = default;
  void                        DoValidate() override final;
  void                        Reset() override final;

protected:
  //Methods
  Double                      GetLengthBasedResult(unsigned age, AgeLength* age_length) override final;

private:
  // Members
  Double                      a50_;
  Double                      ato95_;
  Double                      alpha_;
};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* INVERSELOGISTIC_H_ */
