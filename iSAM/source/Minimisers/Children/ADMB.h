/**
 * @file ADMB.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 19/04/2013
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
#ifdef USE_ADMB
#ifndef MINIMISERS_ADMB_H_
#define MINIMISERS_ADMB_H_

namespace isam {
namespace minimisers {

class ADMB {
public:
  ADMB();
  virtual ~ADMB();
};

} /* namespace minimisers */
} /* namespace isam */
#endif /* MINIMISERS_ADMB_H_ */
#endif /* ADMB */
