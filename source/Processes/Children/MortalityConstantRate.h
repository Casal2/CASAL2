/**
 * @file MortalityConstantRate.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef MORTALITYCONSTANTRATE_H_
#define MORTALITYCONSTANTRATE_H_

// Headers
#include "Processes/Process.h"

// namespaces
namespace isam {
namespace processes {

/**
 * Class Definition
 */
class MortalityConstantRate : public isam::Process {
public:
  // Methods
  MortalityConstantRate();
  virtual                     ~MortalityConstantRate() = default;
  void                        Validate();

private:
  // Members
  vector<string>              category_names_;
  vector<double>              m_;
  vector<string>              selectivity_names_;
};

} /* namespace processes */
} /* namespace isam */
#endif /* MORTALITYCONSTANTRATE_H_ */
