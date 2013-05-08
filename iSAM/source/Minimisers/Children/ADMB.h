/*
 * ADMB.h
 *
 *  Created on: 7/05/2013
 *      Author: Admin
 */
#ifdef USE_AUTODIFF
#ifndef MINIMISERS_ADMB_H_
#define MINIMISERS_ADMB_H_

// Headers
#include "Minimisers/Minimiser.h"
#include "Utilities/Types.h"

// Namespaces
namespace isam {
namespace minimisers {

using isam::utilities::Double;

class ADMB : public isam::Minimiser {
public:
  ADMB();
  virtual ~ADMB() = default;
  void                        Validate();
  void                        Execute();
};

} /* namespace minimisers */
} /* namespace isam */
#endif /* ADMB_H_ */
#endif /* AUTODIFF */
