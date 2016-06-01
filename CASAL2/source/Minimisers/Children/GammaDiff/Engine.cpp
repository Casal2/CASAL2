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

#include "FMM.h"
#include "Utilities/DoubleCompare.h"

// namespaces
namespace niwa {
namespace minimisers {
namespace gammadiff {

// Namespace
using namespace std;
namespace dc = utilities::doublecompare;

//**********************************************************************
// Engine::Engine()
// Default Constructor
//**********************************************************************
Engine::Engine() {
}

//**********************************************************************
// void Engine::condassign( double &res, const double &cond, const double &arg1, const double &arg2 ) {
// Conditional Assignment
//**********************************************************************
void Engine::condAssign(double &res, const double &cond, const double &arg1, const double &arg2) {
  res = (cond) > 0 ? arg1 : arg2;
}

//**********************************************************************
// void Engine::condassign( double &res, const double &cond, const double &arg)
// Conditional Assignment
//**********************************************************************
void Engine::condAssign(double &res, const double &cond, const double &arg) {
  res = (cond) > 0 ? arg : res;
}

//**********************************************************************
// double Engine::boundp(const double& xx, double fmin, double fmax, double& fpen) {
// Boundary Pin
//**********************************************************************
double Engine::unScaleValue(const double& value, double min, double max) {
  // courtesy of AUTODIF - modified to correct error -
  // penalty on values outside [-1,1] multiplied by 100 as of 14/1/02.
  double t = 0.0;
  double y = 0.0;

  t = min + (max - min) * (sin(value * 1.57079633) + 1) / 2;
  this->condAssign(y, -.9999 - value, (value + .9999) * (value + .9999), 0);
  dPenalty += y;
  this->condAssign(y, value - .9999, (value - .9999) * (value - .9999), 0);
  dPenalty += y;
  this->condAssign(y, -1 - value, 1e5 * (value + 1) * (value + 1), 0);
  dPenalty += y;
  this->condAssign(y, value - 1, 1e5 * (value - 1) * (value - 1), 0);
  dPenalty += y;

  return (t);
}

//**********************************************************************
// double Engine::boundpin(double y, double fmin, double fmax)
// Boundary Pin
//**********************************************************************
double Engine::scaleValue(double value, double min, double max) {
  if (dc::IsEqual(value, min))
    return -1;
  else if (dc::IsEqual(value, max))
    return 1;

  return asin(2 * (value - min) / (max - min) - 1) / 1.57079633;
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
    if (dc::IsEqual(vLowerBounds[i], vUpperBounds[i]))
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
    if (dc::IsEqual(vLowerBounds[i], vUpperBounds[i]))
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
double Engine::optimise_finite_differences(gammadiff::CallBack& objective, vector<double>& StartValues, vector<double>& LowerBounds,
    vector<double>& UpperBounds, int& convergence, int& iMaxIter, int& iMaxFunc, double dGradTol,
    double **pOptimiseHessian, int untransformedHessians, double dStepSize) {

  // Variables
  int       iVectorSize   = (int)StartValues.size();
  double    dScore        = 0.0;

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
      LOG_MEDIUM() << "Current estimates: ";
      for (int i = 0; i < iVectorSize; ++i) {
        LOG_MEDIUM() << vCurrentValues[i] << " ";
      }
      LOG_MEDIUM() << "Objective function value: " << dScore;
      dScore += dPenalty; // Bound penalty
    }

    // Gradient Required
    // This will loop through each variable changing it once
    // to see how the other variables change.
    // There-by generating our co-variance
    if (clMinimiser.getResult() >= 1) { // 1 = Gradient Required
      long double dOrigValue;
      long double dStepSizeI;
      long double dScoreI;

      for (int i = 0; i < iVectorSize; ++i) {
        if (dc::IsEqual(vLowerBounds[i], vUpperBounds[i])) {
          vGradientValues[i] = 0.0;

        } else {
          // Workout how much to change the variable by
          dStepSizeI  = dStepSize * ((vScaledValues[i] > 0) ? 1 : -1);

          // Backup Orig Value, and Assign New Var
          dOrigValue        = vScaledValues[i];
          vScaledValues[i]  += dStepSizeI;
          dStepSizeI        = vScaledValues[i] - dOrigValue;

          dPenalty = 0.0;
          buildCurrentValues();

          dScoreI = objective(vCurrentValues);
          dScoreI += dPenalty;

          // Populate Gradient, and Restore Orig Value
          vGradientValues[i]  = (dScoreI - dScore) / dStepSizeI;
          vScaledValues[i]    = dOrigValue;
        }
      }
      // Gradient Finished
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
    StartValues[i] = vCurrentValues[i];
  }

  dScore = objective(vCurrentValues);

  // Generate our Hessian
  if (pOptimiseHessian != 0) {
    double **L              = new double*[iVectorSize];
    double **LT             = new double*[iVectorSize];

    for (int i = 0; i < iVectorSize; ++i) {
      L[i]  = new double[iVectorSize];
      LT[i] = new double[iVectorSize];
    }

    for (int i = 0; i < iVectorSize; ++i) {
      for (int j = 0; j < iVectorSize; ++j) {
        L[i][j] = 0.0;
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
        for (int k = 0; k < iVectorSize; ++k)
          dMulti += (L[i][k] * LT[k][j]);

        pOptimiseHessian[i][j] = dMulti;
      }
    }

    if (untransformedHessians) {
      double *dGradBoundP = new double[iVectorSize];
      for (int i = 0; i < iVectorSize; ++i)
        dGradBoundP[i] = 0.0;

      for (int i = 0; i < iVectorSize; ++i) {
        double dDiv   = ((vStartValues[i]-vLowerBounds[i]) / (vUpperBounds[i]-vLowerBounds[i]));
        double dProd  = (2 * dDiv - 1) * (2 * dDiv - 1);
        double dSqrt  = sqrt(dc::ZeroFun(1-dProd));
        double dProd2 = (vUpperBounds[i] - vLowerBounds[i]) * dSqrt;
        dGradBoundP[i] = (4/3.14159265)/dProd2;
      }

      for (int i = 0; i < iVectorSize; ++i) {
        if (dGradBoundP[i] != dGradBoundP[i]) // NaN
          dGradBoundP[i] = 0.0;
      }

      for (int i = 0; i < iVectorSize; ++i)
        for (int j = 0; j < iVectorSize; ++j) {
          pOptimiseHessian[i][j] *= dGradBoundP[i];
          pOptimiseHessian[j][i] *= dGradBoundP[i];
        }

      delete [] dGradBoundP;
    }

    for (int i = 0; i < iVectorSize; ++i) {
      delete [] L[i];
      delete [] LT[i];
    }

    delete [] L;
    delete [] LT;
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
Engine::~Engine() {
}

} /* namespace gammadiff */
} /* namespace minimisers */
} /* namesapce niwa */
#endif
