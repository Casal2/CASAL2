/**
 * @file DeltaDiff.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 17/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This minimiser is a deterministic numerical differences minimisers
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef USE_AUTODIFF
#ifndef MINIMISERS_DELTADIFF_H_
#define MINIMISERS_DELTADIFF_H_

// headers
#include "../../Minimisers/Minimiser.h"

// namespaces
namespace niwa {
namespace minimisers {

//**********************************************************************
//
//
//**********************************************************************
class DeltaDiff : public niwa::Minimiser {
public:
  // Methods
  DeltaDiff(shared_ptr<Model> model);
  virtual ~DeltaDiff() = default;
  void DoValidate() final{};
  void DoBuild() final{};
  void DoReset() final{};
  void Execute() final { LOG_CODE_ERROR() << "Execute is bad"; }
  void ExecuteThreaded(shared_ptr<ThreadPool> thread_pool) final;

private:
  // Members
  int    max_iterations_;
  int    max_evaluations_;
  double gradient_tolerance_;
  double step_size_;
};

} /* namespace minimisers */
} /* namespace niwa */

#endif /* MINIMISERS_DELTADIFF_H_ */
#endif /* USE_AUTODIFF */
