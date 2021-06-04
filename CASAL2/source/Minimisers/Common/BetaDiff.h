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
#ifdef USE_AUTODIFF
#ifdef USE_BETADIFF
#ifndef BETADIFF_H_
#define BETADIFF_H_

// headers
#include "../../Minimisers/Minimiser.h"
#include "../../Utilities/Types.h"

// namespaces
namespace niwa {
namespace minimisers {

using niwa::utilities::Double;

/**
 * class definition
 */
class BetaDiff : public niwa::Minimiser {
public:
  // Methods
  explicit BetaDiff(shared_ptr<Model> model);
  virtual ~BetaDiff() = default;
  void DoValidate() override final{};
  void DoBuild() override final{};
  void DoReset() override final{};
  void Execute() override final;

private:
  // Members
  int    max_iterations_;
  int    max_evaluations_;
  double gradient_tolerance_;
};

}  // namespace minimisers
} /* namespace niwa */
#endif /* BETADIFF_H_ */
#endif /* USE_BETADIFF */
#endif /* USE_AUTODIFF */
