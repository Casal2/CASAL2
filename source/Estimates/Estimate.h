/**
 * @file Estimate.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class represents a currently loaded estimate. Each
 * estimate is tied to a specific registered estimable in the
 * system. There is a 1:1 relationship between estimate and estimable
 * (excluding when the 'same' keyword is used).
 *
 * This object is not constructed directly from a configuration file
 * block, instead the isam::estimates::Info is used to create this.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

#ifndef ESTIMATE_H_
#define ESTIMATE_H_

namespace isam {

class Estimate {
public:
  Estimate();
  virtual ~Estimate();
};

} /* namespace isam */
#endif /* ESTIMATE_H_ */
