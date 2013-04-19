/**
 * @file UniformLog.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This prior will return the log(param) when asked for a score
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PRIORS_UNIFORMLOG_H_
#define PRIORS_UNIFORMLOG_H_

// Headers
#include "Priors/Prior.h"

// Namespaces
namespace isam {
namespace priors {

/**
 * Class definition
 */
class UniformLog : public isam::Prior {
public:
  // Methods
  UniformLog();
  virtual                     ~UniformLog() noexcept(true);
  Double                      GetScore(Double param) override final;
};

} /* namespace priors */
} /* namespace isam */
#endif /* PRIORS_UNIFORMLOG_H_ */
