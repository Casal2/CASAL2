/**
 * @file Process.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Process penalties (i.e., special flagged penalties like the catch limit penalty.
 * Has an arbitrary multiplier (default=1) as well.
 */
#ifndef PENALTIES_PROCESS_H_
#define PENALTIES_PROCESS_H_

// headers
#include "Penalties/Penalty.h"

// namespaces
namespace niwa {
namespace penalties {

/**
 * Class definition
 */
class Process : public niwa::Penalty {
public:
  // methods
  Process(Model* model);
  virtual                     ~Process() = default;
  void                        Trigger(const string& source_label, Double value_1, Double value_2);
  Double                      GetScore() override final { return 0.0; }
protected:
  // methods
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };

private:
  // members
  double                      multiplier_;
  bool                        log_scale_;
};
} /* namespace penalties */
} /* namespace niwa */

#endif /* PENALTIES_PROCESS_H_ */
