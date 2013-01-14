/**
 * @file Increasing.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/01/2013
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
#ifndef INCREASING_H_
#define INCREASING_H_

// Headers
#include "Selectivities/Selectivity.h"

// Namespaces
namespace isam {
namespace selectivities {

/**
 * Class definition
 */
class Increasing : public isam::Selectivity {
public:
  // Methods
  Increasing();
  virtual                     ~Increasing() {};
  void                        Validate() override final;
  void                        Reset() override final;

private:
  // Members
  unsigned                    low_ = 0.0;
  unsigned                    high_ = 0.0;
  vector<double>              v_;
  double                      alpha_ = 1.0;
};

} /* namespace selectivities */
} /* namespace isam */
#endif /* INCREASING_H_ */
