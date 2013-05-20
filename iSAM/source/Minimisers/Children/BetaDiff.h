/**
 * @file BetaDiff.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 20/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This minimiser uses the BetaDiff library taken
 * from the CASAL
 */
#ifdef USE_BETADIFF
#ifndef BETADIFF_H_
#define BETADIFF_H_

// headers
#include "Minimisers/Minimiser.h"
#include "Utilities/Types.h"

// namespaces
namespace isam {
namespace minimisers {

using isam::utilities::Double;

/**
 * class definition
 */
class BetaDiff : public isam::Minimiser {
public:
  // Methods
  BetaDiff();
  virtual                     ~BetaDiff() = default;
  void                        Validate();
  void                        Execute();
};

} /* namespace reports */
} /* namespace isam */
#endif /* BETADIFF_H_ */
#endif /* USE_BETADIFF */
