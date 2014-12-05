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
namespace niwa {

using niwa::utilities::Double;

/**
 * Class definitiom
 */
class Catchability : public niwa::base::Object {
public:
  // Methods
  Catchability();
  virtual                     ~Catchability() = default;
  void                        Validate();
  void                        Build() { };
  void                        Reset() { };
  virtual void                DoValidate() = 0;

  // Accessors
  virtual Double              q() const = 0;
};

// Typdef
typedef boost::shared_ptr<niwa::Catchability> CatchabilityPtr;

} /* namespace niwa */
#endif /* CATCHABILITY_H_ */
