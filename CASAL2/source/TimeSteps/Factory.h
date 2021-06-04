/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef TIMESTEPS_FACTORY_H_
#define TIMESTEPS_FACTORY_H_

// Headers
#include "../TimeSteps/TimeStep.h"

namespace niwa {
namespace timesteps {

class Factory {
public:
  // methods
  static TimeStep* Create(shared_ptr<Model> model, const string& object_type, const string& sub_type);

private:
  // methods
  Factory()  = delete;
  ~Factory() = delete;
};

} /* namespace timesteps */
} /* namespace niwa */
#endif /* TIMESTEPS_FACTORY_H_ */
