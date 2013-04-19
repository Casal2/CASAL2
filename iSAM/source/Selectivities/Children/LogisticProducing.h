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
namespace isam {
namespace selectivities {

/**
 * Class definition
 */
class LogisticProducing : public isam::Selectivity {
public:
  // Methods
  LogisticProducing();
  explicit LogisticProducing(ModelPtr model);
  virtual                     ~LogisticProducing() {};
  void                        Validate() override final;
  void                        Reset() override final;

private:
  // Members
  unsigned                    low_ = 0;
  unsigned                    high_ = 0;
  Double                      a50_ = 0.0;
  Double                      aTo95_ = 0.0;
  Double                      alpha_ = 1.0;
};

} /* namespace selectivities */
} /* namespace isam */
#endif /* LOGISTICPRODUCING_H_ */
