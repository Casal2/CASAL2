/**
 * @file Logistic.h
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
#ifndef LOGISTIC_H_
#define LOGISTIC_H_

// Headers
#include "Selectivities/Selectivity.h"

// Namespaces
namespace isam {
namespace selectivities {

/**
 * Class Definition
 */
class Logistic : public isam::Selectivity {
public:
  // Methods
  Logistic();
  virtual                     ~Logistic() {};
  void                        Validate() override final;
  void                        Reset() override final;

private:
  // Members
  Double                      a50_ = 0.0;
  Double                      aTo95_ = 0.0;
  Double                      alpha_ = 1.0;
};

} /* namespace selectivities */
} /* namespace isam */
#endif /* LOGISTIC_H_ */
