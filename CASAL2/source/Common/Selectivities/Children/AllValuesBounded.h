/**
 * @file AllValuesBounded.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/01/2013
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
#ifndef ALLVALUESBOUNDED_H_
#define ALLVALUESBOUNDED_H_

// Headers
#include "Common/Selectivities/Selectivity.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Class Definition
 */
class AllValuesBounded : public niwa::Selectivity {
public:
  // Methods
  explicit AllValuesBounded(Model* model);
  virtual                     ~AllValuesBounded() = default;
  void                        DoValidate() override final;
  void                        Reset() override final;

protected:
  //Methods
  Double                      GetLengthBasedResult(unsigned age, AgeLength* age_length) override final;

private:
  // Members
  unsigned                    low_;
  unsigned                    high_;
  vector<Double>              v_;
};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* ALLVALUESBOUNDED_H_ */
