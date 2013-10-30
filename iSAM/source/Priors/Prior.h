/**
 * @file Prior.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
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
#ifndef PRIOR_H_
#define PRIOR_H_

// Headers
#include "BaseClasses/Object.h"
#include "Utilities/Types.h"

// Namespaces
namespace isam {

using isam::utilities::Double;

/**
 * Class Definition
 */
class Prior : public isam::base::Object {
public:
  // Methods
  Prior();
  virtual                     ~Prior() = default;
  void                        Validate();
  void                        Build() {};
  void                        Reset() {};


  // pure methods
  virtual void                DoValidate() = 0;
  virtual Double              GetScore(Double param) = 0;

protected:
  // Members
  string                      type_ = "";
  Double                      score_ = 0.0;
};

// Typedef
typedef boost::shared_ptr<isam::Prior> PriorPtr;


} /* namespace isam */
#endif /* PRIOR_H_ */
