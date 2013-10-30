/**
 * @file NormalLog.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/03/2013
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
#ifndef PRIORS_NORMALLOG_H_
#define PRIORS_NORMALLOG_H_

// Headers
#include "Priors/Prior.h"

// Namespaces
namespace isam {
namespace priors {

/**
 * Class definition
 */
class NormalLog : public isam::Prior {
public:
  // Methods
  NormalLog();
  virtual                     ~NormalLog() = default;
  void                        DoValidate() override final;
  Double                      GetScore(Double param) override final;

private:
  // Members
  Double                      mu_;
  Double                      sigma_;
};

} /* namespace priors */
} /* namespace isam */
#endif /* PRIORS_NORMALLOG_H_ */
