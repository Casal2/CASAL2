/**
 * @file LogisticProducing.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef LOGISTICPRODUCING_H_
#define LOGISTICPRODUCING_H_

// Headers
#include "Selectivities/Selectivity.h"

// namespaces
namespace niwa {
namespace selectivities {

/**
 * Class definition
 */
class LogisticProducing : public niwa::Selectivity {
public:
  // Methods
  explicit LogisticProducing(Model* model);
  virtual                     ~LogisticProducing() = default;
  void                        DoValidate() override final;
  void                        RebuildCache() override final;

protected:
  //Methods
  Double                      GetLengthBasedResult(unsigned age, AgeLength* age_length, unsigned year = 0, int time_step_index = -1) override final;

private:
  // Members
  unsigned                    low_;
  unsigned                    high_;
  Double                      a50_;
  Double                      ato95_;
  Double                      alpha_;
};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* LOGISTICPRODUCING_H_ */
