//============================================================================
// Name        : GammaDiffEngine.h
// Author      : S.Rasmussen
// Date        : 21/08/2008
// Copyright   : Copyright NIWA Science ©2008 - www.niwa.co.nz
// Description :
// $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
//============================================================================
#ifndef USE_AUTODIFF
#ifndef MINIMISERS_GAMMADIFF_ENGINE_H_
#define MINIMISERS_GAMMADIFF_ENGINE_H_

// Global Headers
#include "Callback.h"

#include <vector>

// namespaces
namespace niwa {
namespace minimisers {
namespace gammadiff {

using std::vector;

/**
 * Class definition
 */
class Engine {
public:
  Engine();
  virtual                     ~Engine();
  double optimise_finite_differences(gammadiff::CallBack& objective, vector<double>& StartValues, vector<double>& LowerBounds,
      vector<double>& UpperBounds, int& convergence, int& iMaxIter, int& iMaxFunc, double dGradTol,
      double **pOptimiseHessian, int untransformedHessians, double dStepSize);

private:
  // Variables
  vector<double>              vStartValues;
  vector<double>              vLowerBounds;
  vector<double>              vUpperBounds;
  vector<double>              vScaledValues;
  vector<double>              vCurrentValues;
  vector<double>              vGradientValues;
  double                      dPenalty;

  // Functions
  void                        buildScaledValues();
  void                        buildCurrentValues();
  double                      scaleValue(double value, double min, double max);
  double                      unScaleValue(const double& value, double min, double max);
  void                        condAssign(double &res, const double &cond, const double &arg1, const double &arg2);
  void                        condAssign(double &res, const double &cond, const double &arg);
};

} /* namespace gammadiff */
} /* namespace minimisers */
} /* namesapce niwa */

#endif /* MINIMISERS_GAMMADIFF_ENGINE_H_ */
#endif
