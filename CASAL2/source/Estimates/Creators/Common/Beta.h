/**
 * @file Beta.h
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
#ifndef ESTIMATES_CREATOR_BETA_H_
#define ESTIMATES_CREATOR_BETA_H_

// headers
#include "../../../Estimates/Creators/Creator.h"

// namespaces
namespace niwa {
namespace estimates {
namespace creators {

/**
 *
 */
class Beta : public estimates::Creator {
public:
  // methods
  Beta() = delete;
  explicit Beta(shared_ptr<Model> model);
  virtual                     ~Beta() = default;
  void                        DoCopyParameters(niwa::Estimate* estimate, unsigned index) override final;

private:
  // members
  vector<Double>              mu_;
  vector<Double>              sigma_;
  vector<Double>              a_;
  vector<Double>              b_;
};

} /* namespace creators */
} /* namespace estimates */
} /* namespace niwa */

#endif /* BETA_H_ */
