/**
 * @file ProportionsAtAge.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/04/2013
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
#ifndef PROPORTIONSATAGE_H_
#define PROPORTIONSATAGE_H_

// Headers
#include "Observations/Observation.h"

// Namespace
namespace isam {
namespace observations {

/**
 * Class Definition
 */
class ProportionsAtAge : public isam::Observation {
public:
  // Methods
  ProportionsAtAge();
  virtual                     ~ProportionsAtAge() = default;
  void                        Validate() override final;
  void                        Build() override final;
  void                        PreExecute() override final;
  void                        Execute() override final;
};

} /* namespace observations */
} /* namespace isam */
#endif /* PROPORTIONSATAGE_H_ */
