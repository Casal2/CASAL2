/**
 * @file Catchability.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/03/2013
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
#ifndef CATCHABILITY_H_
#define CATCHABILITY_H_

// Headers
#include "BaseClasses/Object.h"
#include "Utilities/Types.h"

// Namespaces
namespace isam {

using isam::utilities::Double;

/**
 * Class definitiom
 */
class Catchability : public isam::base::Object {
public:
  // Methods
  Catchability();
  virtual                     ~Catchability();
  void                        Validate();
  void                        Build() { };

  // Accessors
  Double                      q() const { return q_; }

private:
  // Members
  Double                      q_;
};

// Typdef
typedef boost::shared_ptr<isam::Catchability> CatchabilityPtr;

} /* namespace isam */
#endif /* CATCHABILITY_H_ */
