/**
 * @file MortalityEvent.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 21/12/2012
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
#ifndef MORTALITYEVENT_H_
#define MORTALITYEVENT_H_

// Headers
#include "Processes/Process.h"

// Namespaces
namespace isam {
namespace processes {

/**
 * Class definition
 */
class MortalityEvent : public isam::Process {
public:
  // Methods
  MortalityEvent();
  virtual                     ~MortalityEvent() = default;
  void                        Validate();

private:
  // Members
  vector<string>              category_names_;
  vector<unsigned>            years_;
  vector<double>              catches_;
  double                      u_max_;
  vector<string>              selectivity_names_;
  string                      penalty_;
};

} /* namespace processes */
} /* namespace isam */
#endif /* MORTALITYEVENT_H_ */
