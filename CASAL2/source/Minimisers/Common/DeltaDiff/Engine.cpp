
/**
 * @file Engine.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 17/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef USE_AUTODIFF
// headers
#include "Engine.h"

#include <math.h>

#include <iomanip>

#include "../../../Minimisers/Common/DeltaDiff/FMM.h"
#include "../../../Translations/Translations.h"
#include "../../../Utilities/Gradient.h"
#include "../../../Utilities/Math.h"
#include "../../../Utilities/Vector.h"

// namespaces
namespace niwa {
namespace minimisers {
namespace deltadiff {

// Namespace
using namespace std;
namespace math     = niwa::utilities::math;
namespace gradient = niwa::utilities::gradient;
using namespace niwa::utilities;

//**********************************************************************
// Engine::Engine()
// Default Constructor
//**********************************************************************
Engine::Engine() {}

//**********************************************************************
// void Engine::condassign( double &res, const double &cond, const double &arg1, const double &arg2 ) {
// Conditional Assignment
//**********************************************************************
void Engine::condAssign(double& res, const double& cond, const double& arg1, const double& arg2) {
  res = (cond) > 0 ? arg1 : arg2;
}

//**********************************************************************
// void Engine::condassign( double &res, const double &cond, const double &arg)
// Conditional Assignment
//**********************************************************************
void Engine::condAssign(double& res, const double& cond, const double& arg) {
  res = (cond) > 0 ? arg : res;
}

constexpr double PI = 3.1415926535897932384626433832795028;
//**********************************************************************
// double Engine::boundp(const double& xx, double fmin, double fmax, double& fpen) {
// Boundary Pin
//**********************************************************************
double Engine::unScaleValue(const double& value, double min, double max) {
  double unscaled = ((atan(value) / PI) + 0.5) * (max - min) + min;
  return unscaled;
}

//**********************************************************************
// double Engine::boundpin(double y, double fmin, double fmax)
// Boundary Pin
//**********************************************************************
double Engine::scaleValue(double value, double min, double max) {
  double scaled = tan(((value - min) / (max - min) - 0.5) * PI);
  return scaled;
}

//**********************************************************************
// void Engine::buildScaledValues()
// Build our scaled values in relation to our bounds
//**********************************************************************
void Engine::buildScaledValues() {
  for (int i = 0; i < (int)vStartValues.size(); ++i) {
    // Check
    if (vStartValues[i] < vLowerBounds[i])
      throw string(GAMMADIFF_LESS_START_LOWER_BOUND);
    if (vStartValues[i] > vUpperBounds[i])
      throw string(GAMMADIFF_GREATER_START_UPPER_BOUND);

    // Boundary-Pinning
    if (math::IsEqual(vLowerBounds[i], vUpperBounds[i]))
      vScaledValues[i] = 0.0;
    else
      vScaledValues[i] = scaleValue(vStartValues[i], vLowerBounds[i], vUpperBounds[i]);
  }
}

//**********************************************************************
// void Engine::buildCurrentValues()
// Build the current test values from our scaled values
//**********************************************************************
void Engine::buildCurrentValues() {
  for (int i = 0; i < (int)vStartValues.size(); ++i) {
    if (math::IsEqual(vLowerBounds[i], vUpperBounds[i]))
      vCurrentValues[i] = vLowerBounds[i];
    else
      vCurrentValues[i] = unScaleValue(vScaledValues[i], vLowerBounds[i], vUpperBounds[i]);
  }
}

//**********************************************************************
// double Engine::optimise_finite_differences(CGammaDiffCallback& objective, vector<double>& StartValues, vector<double>& LowerBounds,
//   vector<double>& UpperBounds, int& convergence, int& iMaxIter, int& iMaxFunc, double dGradTol,
//   double **pOPTIMIZEHessian, int untransformedHessians, double dStepSize)
// OPTIMIZE our function
//**********************************************************************
double Engine::optimise_finite_differences(deltadiff::CallBack& objective, vector<double>& StartValues, vector<double>& LowerBounds, vector<double>& UpperBounds, int& convergence,
                                           int& iMaxIter, int& iMaxFunc, double dGradTol, double** pOptimiseHessian, int untransformedHessians, double dStepSize) {
  // Variables
  int    iVectorSize = (int)StartValues.size();
  double dScore      = 0.0;

  // Assign Our Vectors
  vStartValues.assign(StartValues.begin(), StartValues.end());
  vLowerBounds.assign(LowerBounds.begin(), LowerBounds.end());
  vUpperBounds.assign(UpperBounds.begin(), UpperBounds.end());

  vScaledValues.resize(iVectorSize);
  vCurrentValues.resize(iVectorSize);
  vGradientValues.resize(iVectorSize);

  // Validate
  if ((int)LowerBounds.size() != iVectorSize)
    throw string(GAMMADIFF_SIZE_LOWER_BOUND_START_VALUE);
  if ((int)UpperBounds.size() != iVectorSize)
    throw string(GAMMADIFF_SIZE_UPPER_BOUND_START_VALUE);

  // Create our Minimiser
  FMM clMinimiser(iVectorSize, iMaxFunc, iMaxIter, dGradTol);

  // Build our Scaled Values
  buildScaledValues();

  // Loop through our Minimiser now
  while (clMinimiser.getResult() >= 0) {
    // Do we need to evaluate objective function again?
    if ((clMinimiser.getResult() == 0) || (clMinimiser.getResult() == 2)) {
      // Reset Variables
      dPenalty = 0.0;

      buildCurrentValues();
      dScore = objective(vCurrentValues);
      std::cerr << "Current estimates: ";
      for (int i = 0; i < iVectorSize; ++i) {
        std::cerr << vCurrentValues[i] << " ";
      }
      std::cerr << "\nObjective function value: " << dScore << "\n";
      dScore += dPenalty;  // Bound penalty
    }

    // Gradient Required
    // This will loop through each variable changing it once
    // to see how the other variables change.
    // There-by generating our co-variance
    if (clMinimiser.getResult() >= 1) {  // 1 = Gradient Required
      LOG_MEDIUM() << "Calculating Gradient";
      // cout << std::setprecision(20) << endl;
      // cout << "-------------- Calling Gradient Calculation" << endl;
      vGradientValues = gradient::Calculate(objective.thread_pool(), vScaledValues, LowerBounds, UpperBounds, dStepSize, dScore, true);
      // Vector_Debug("Scaled Values: ", vScaledValues);
      // Vector_Debug("Gradient: ", vGradientValues);
      // cout << "Step Size: " << dStepSize << endl;
      // cout << "dScore: " << dScore << endl;
      // cout << "-------------- END Calling Gradient Calculation" << endl;
    }
    // Call our Function Minimiser
    clMinimiser.fMin(vScaledValues, dScore, vGradientValues);
  }

  if (clMinimiser.getResult() == -3) {
    LOG_MEDIUM() << GAMMADIFF_CONVERGENCE_UNCLEAR;
  } else if (clMinimiser.getResult() == -2) {
    LOG_MEDIUM() << GAMMADIFF_EVALUATIONS_MAX;
  } else if (clMinimiser.getResult() == -1) {
    LOG_MEDIUM() << GAMMADIFF_CONVERGENCE_SUCCESSFUL;
  } else {
    LOG_MEDIUM() << GAMMADIFF_RETURN_VALUE_UNKNOWN << clMinimiser.getResult();
  }

  for (int i = 0; i < iVectorSize; ++i) {
    vCurrentValues[i] = unScaleValue(vScaledValues[i], vLowerBounds[i], vUpperBounds[i]);
  }

  for (int i = 0; i < iVectorSize; ++i) {
    vStartValues[i] = vCurrentValues[i];
    StartValues[i]  = vCurrentValues[i];
  }

  dScore = objective(vCurrentValues);

  // Generate our Hessian
  if (pOptimiseHessian != 0) {
    double** L  = new double*[iVectorSize];
    double** LT = new double*[iVectorSize];

    for (int i = 0; i < iVectorSize; ++i) {
      L[i]  = new double[iVectorSize];
      LT[i] = new double[iVectorSize];
    }

    for (int i = 0; i < iVectorSize; ++i) {
      for (int j = 0; j < iVectorSize; ++j) {
        L[i][j]  = 0.0;
        LT[i][j] = 0.0;
      }
    }

    for (int i = 0; i < iVectorSize; ++i) {
      for (int j = 0; j <= i; ++j) {
        L[i][j] = LT[j][i] = clMinimiser.getHessianValue(i, j);
      }
    }

    for (int i = 0; i < iVectorSize; ++i) {
      for (int j = 0; j < iVectorSize; ++j) {
        double dMulti = 0.0;

        // Loop Through
        for (int k = 0; k < iVectorSize; ++k) dMulti += (L[i][k] * LT[k][j]);

        pOptimiseHessian[i][j] = dMulti;
      }
    }

    if (untransformedHessians) {
      double* dGradBoundP = new double[iVectorSize];
      for (int i = 0; i < iVectorSize; ++i) dGradBoundP[i] = 0.0;

      for (int i = 0; i < iVectorSize; ++i) {
        double dDiv    = ((vStartValues[i] - vLowerBounds[i]) / (vUpperBounds[i] - vLowerBounds[i]));
        double dProd   = (2 * dDiv - 1) * (2 * dDiv - 1);
        double dSqrt   = sqrt(math::ZeroFun(1 - dProd));
        double dProd2  = (vUpperBounds[i] - vLowerBounds[i]) * dSqrt;
        dGradBoundP[i] = (4 / 3.14159265) / dProd2;
      }

      for (int i = 0; i < iVectorSize; ++i) {
        if (dGradBoundP[i] != dGradBoundP[i])  // NaN
          dGradBoundP[i] = 0.0;
      }

      for (int i = 0; i < iVectorSize; ++i)
        for (int j = 0; j < iVectorSize; ++j) {
          pOptimiseHessian[i][j] *= dGradBoundP[i];
          pOptimiseHessian[j][i] *= dGradBoundP[i];
        }

      delete[] dGradBoundP;
    }

    for (int i = 0; i < iVectorSize; ++i) {
      delete[] L[i];
      delete[] LT[i];
    }

    delete[] L;
    delete[] LT;
  }

  convergence = clMinimiser.getResult() + 2;
  iMaxIter    = clMinimiser.getIters();
  iMaxFunc    = clMinimiser.getEvals();

  return dScore;
}

//**********************************************************************
// Engine::~Engine()
// Default Destructor
//**********************************************************************
Engine::~Engine() {}

} /* namespace deltadiff */
} /* namespace minimisers */
}  // namespace niwa
#endif
