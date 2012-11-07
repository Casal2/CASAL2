/**
 * @file Ageing.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This is a simple ageing process. It's responsible for ageing all
 * fish by 1 each time it's called.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef AGEING_H_
#define AGEING_H_

// Headers
#include "Processes/Process.h"

namespace isam {
namespace processes {

class Ageing : public isam::Process {
public:
  // Methods
  Ageing();
  virtual                     ~Ageing();

};

} /* namespace processes */
} /* namespace isam */
#endif /* AGEING_H_ */
