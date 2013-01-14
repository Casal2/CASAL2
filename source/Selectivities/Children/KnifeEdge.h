/**
 * @file KnifeEdge.h
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
#ifndef KNIFEEDGE_H_
#define KNIFEEDGE_H_

// Headers
#include "Selectivities/Selectivity.h"

// Namespaces
namespace isam {
namespace selectivities {

/**
 * Class Definition
 */
class KnifeEdge : public isam::Selectivity {
public:
  // Methods
  KnifeEdge();
  virtual                     ~KnifeEdge() {};
  void                        Validate() override final;
  void                        Reset() override final;

private:
  // Members
  Double                      edge_ = 0.0;
  Double                      alpha_ = 1.0;

};

} /* namespace selectivities */
} /* namespace isam */
#endif /* KNIFEEDGE_H_ */
