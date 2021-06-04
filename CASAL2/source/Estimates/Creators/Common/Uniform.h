/**
 * @file Uniform.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef ESTIMATES_CREATORS_UNIFORM_H_
#define ESTIMATES_CREATORS_UNIFORM_H_

// headers
#include "../../../Estimates/Creators/Creator.h"

// namespaces
namespace niwa {
namespace estimates {
namespace creators {

/**
 *
 */
class Uniform : public estimates::Creator {
public:
  // methods
  Uniform() = delete;
  explicit Uniform(shared_ptr<Model> model);
  virtual ~Uniform() = default;
  void DoCopyParameters(niwa::Estimate* estimate, unsigned index) override final{};
};

} /* namespace creators */
} /* namespace estimates */
} /* namespace niwa */

#endif /* ESTIMATES_CREATORS_UNIFORM_H_ */
