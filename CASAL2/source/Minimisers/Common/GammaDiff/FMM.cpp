/**
 * @file FMM.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 17/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
// Headers
#include "FMM.h"

#include <cmath>
#include <iostream>

#include "../../../Logging/Logging.h"

// namespaces
namespace niwa {
namespace minimisers {
namespace gammadiff {

//**********************************************************************
// FMM::FMM(int n)
// Default Constructor
//**********************************************************************
FMM::FMM(int vectorSize, int maxFunc, int maxQuasiSteps, double gradTol) {
  // Assign the Variables
  iVectorSize             = vectorSize;
  iMaxFunc                = maxFunc;
  iMaxQuasiSteps          = maxQuasiSteps;
  dGradTol                = gradTol;
  bMaxStepTaken           = false;
  iMaxSteps               = 200;
  iLinearSearchIters      = 0;
  dPreviousScore          = 0.0;
  iIters                  = 0;
  iEvals                  = 1;
  iRet                    = 2;  // Not called yet
  dNewtonStepLength       = 0.0;
  dAlpha                  = 0.0001;
  dLambda                 = 0.0;
  dLambdaMin              = 0.0;
  dLambdaTemp             = 0.0;
  dPreviousLambda         = 0.0;
  dCurrentTolerance       = 0.0;
  dMaxStep                = 1000.0;
  pPreviousCandidates     = new long double[iVectorSize];
  pLastPreviousCandidates = new long double[iVectorSize];
  pTrialValues            = new long double[iVectorSize];
  pCholeskyVector         = new long double[iVectorSize];
  pGradient               = new long double[iVectorSize];
  pLastGradient           = new long double[iVectorSize];
  pTemp                   = new long double[iVectorSize];

  // Build the Hessian Construct Matrix
  pHessianConstruct = new long double*[iVectorSize];
  for (int i = 0; i < iVectorSize; ++i) pHessianConstruct[i] = new long double[iVectorSize];

  // Calculate the machine Epsilon
  dEpsilon = 1.0;
  do {
    dEpsilon = dEpsilon / 2;
  } while ((1 + dEpsilon) != 1);

  // Set more variables
  dEpsilon = 2 * dEpsilon;
  dStepTol = pow((double)dEpsilon, 2. / 3.);
}

//**********************************************************************
// void FMM::jacrotate(int i, long double a, long double b, long double** M)
// Jacobian Rotate
//**********************************************************************
void FMM::jacrotate(int i, long double a, long double b, long double** M) {
  // Used by the fmm minimiser.
  long double c, s, w, y, den;

  if (a == 0) {
    c = 0;
    if (b > 0) {
      s = 1;
    } else {
      s = -1;
    }
  } else {
    den = sqrt(a * a + b * b);
    c   = a / den;
    s   = b / den;
  }

  for (int j = i; j < iVectorSize; ++j) {
    y           = M[i][j];
    w           = M[i + 1][j];
    M[i][j]     = c * y - s * w;
    M[i + 1][j] = s * y + c * w;
  }
}

//**********************************************************************
//
//
//**********************************************************************
void FMM::qrupdate(long double* u, long double* v, long double** M) {
  // Used by the fmm minimiser.
  // note qrupdate is called with u=t, v=u...!
  int k;

  for (int i = 1; i < iVectorSize; ++i) M[i][i - 1] = 0;

  k = iVectorSize - 1;
  while (u[k] == 0 && k > 0) k--;

  for (int i = k - 1; i >= 0; i--) {
    jacrotate(i, u[i], -u[i + 1], M);

    if (u[i] == 0) {
      u[i] = fabs(u[i + 1]);
    } else {
      u[i] = sqrt(pow(u[i], 2) + pow(u[i + 1], 2));
    }
  }

  for (int i = 0; i < iVectorSize; ++i) {
    M[0][i] += u[0] * v[i];
  }

  for (int i = 0; i < k; ++i) jacrotate(i, M[i][i], -M[i + 1][i], M);
}

//**********************************************************************
//
// bfgsfac(nvar, x, xlast, g, glast, eps, eps, L);
//**********************************************************************
void FMM::bfgsfac(long double* x, long double* xlast, long double* g, long double* glast, long double** pHessianConstruct) {
  // Used by the fmm minimiser.
  // only the lower triangle of L comes out correct; the upper triangle is used for storage. Makes
  // me a bit uncomfortable but I don't think the upper triangle gets used by any of my other functions.

  long double  nu  = dEpsilon;
  long double  eps = dEpsilon;
  long double *s, *y, *t, *u;
  s = new long double[iVectorSize];
  y = new long double[iVectorSize];
  t = new long double[iVectorSize];
  u = new long double[iVectorSize];
  int         skipupdate;
  long double temp1, temp2, temp3, alpha;

  temp1 = 0;
  temp2 = 0;
  temp3 = 0;

  for (int i = 0; i < iVectorSize; ++i) {
    s[i] = x[i] - xlast[i];
    y[i] = g[i] - glast[i];
    temp1 += s[i] * y[i];
    temp2 += pow(s[i], 2);
    temp3 += pow(y[i], 2);
  }

  temp2 = sqrt(temp2);
  temp3 = sqrt(temp3);

  if (temp1 >= (sqrt(eps) * temp2 * temp3)) {
    temp2 = 0;

    for (int i = 0; i < iVectorSize; ++i) {
      t[i] = 0.0;

      for (int j = i; j < iVectorSize; ++j) t[i] += pHessianConstruct[j][i] * s[j];

      temp2 += t[i] * t[i];
    }

    alpha      = sqrt((temp1 / temp2));
    skipupdate = 1;

    for (int i = 0; i < iVectorSize; ++i) {
      temp3 = 0;

      for (int j = 0; j <= i; ++j) temp3 += pHessianConstruct[i][j] * t[j];

      if (fabs(y[i] - temp3) >= (nu * fmax(fabs(g[i]), fabs(glast[i])))) {
        skipupdate = 0;
      }
      u[i] = y[i] - alpha * temp3;
    }

    if (!skipupdate) {
      temp3 = 1 / sqrt(temp1 * temp2);

      for (int i = 0; i < iVectorSize; ++i) t[i] *= temp3;

      for (int i = 1; i < iVectorSize; ++i) {
        for (int j = 0; j < i; ++j) pHessianConstruct[j][i] = pHessianConstruct[i][j];
      }

      qrupdate(t, u, pHessianConstruct);

      for (int i = 1; i < iVectorSize; ++i) {
        for (int j = 0; j < i; ++j) pHessianConstruct[i][j] = pHessianConstruct[j][i];
      }
    }
  }

  delete[] s;
  delete[] y;
  delete[] t;
  delete[] u;
}

//**********************************************************************
// void FMM::CholeskyDecomposition(long double* g, long double** L, long double* p)
// http://en.wikipedia.org/wiki/Cholesky_decomposition
//**********************************************************************
void FMM::CholeskyDecomposition(long double* Gradient, long double** LowerTriangle, long double* CholeskyVector) {
  long double* y    = new long double[iVectorSize];
  long double  temp = 0.0;

  y[0] = Gradient[0] / LowerTriangle[0][0];
  for (int i = 1; i < iVectorSize; ++i) {
    temp = 0;
    for (int j = 0; j < i; ++j) {
      temp += LowerTriangle[i][j] * y[j];
    }
    y[i] = (Gradient[i] - temp) / LowerTriangle[i][i];
  }

  CholeskyVector[iVectorSize - 1] = y[iVectorSize - 1] / LowerTriangle[iVectorSize - 1][iVectorSize - 1];

  for (int i = iVectorSize - 2; i >= 0; --i) {
    temp = 0.0;
    for (int j = i + 1; j < iVectorSize; ++j) {
      temp += LowerTriangle[j][i] * CholeskyVector[j];
    }
    CholeskyVector[i] = (y[i] - temp) / LowerTriangle[i][i];
  }

  delete[] y;
}

//**********************************************************************
// void FMM::fmin(vector<long double>& xvec, long double& func, vector<long double>& gvec)
//
//**********************************************************************
void FMM::fMin(vector<double>& Candidates, double& Score, vector<double>& Gradient) {
  if (iRet == 0) {  // bringing a function call for the linear search

    iEvals++;

    // have exceeded maximum no. of function evaluations
    if (iEvals > iMaxFunc) {
      iRet = -2;
      LOG_MEDIUM() << "FMM: Too many function evaluations (" << iEvals << ")";

      for (int i = 0; i < iVectorSize; ++i) Candidates[i] = pPreviousCandidates[i];

      return;
    }

    iLinearSearchIters++;
    if (iLinearSearchIters > iMaxSteps) {
      iRet = -3;
      LOG_MEDIUM() << "FMM: Too many loops in linear search (" << iLinearSearchIters << ")";

      // Go back to last accepted candidates
      for (int i = 0; i < iVectorSize; ++i) Candidates[i] = pPreviousCandidates[i];

      return;
    }

    // Set our Previous Score
    dPreviousScore = Score;

    // Is this point acceptable?
    if (dPreviousScore <= (dPreviousScore2 + dAlpha * dLambda * dSlope)) {
      // Proceed with Linear Step
      for (int i = 0; i < iVectorSize; i++) pLastPreviousCandidates[i] = pPreviousCandidates[i];  // Populate LastPrevious

      // Have we taken Max Step?
      if ((dLambda >= 1) && (dNewtonStepLength > (0.99 * dMaxStep)))
        bMaxStepTaken = true;

      iRet = 1;
      return;
    }

    // Is StepSize too Small?
    if (dLambda < dLambdaMin) {
      iRet = -3;
      LOG_MEDIUM() << "FMM Linear step size too small (" << (double)dLambda << ")";
      return;
    }

    // Build Lambda
    if (dLambda == 1) {
      dLambdaTemp = -dSlope / (2 * (dPreviousScore - dPreviousScore2 - dSlope));
    } else {
      dParamA = (1 / (dLambda - dPreviousLambda))
                * ((1 / (pow(dLambda, 2))) * (dPreviousScore - dPreviousScore2 - dLambda * dSlope)
                   + (-1 / pow(dPreviousLambda, 2)) * (dLastPreviousScore - dPreviousScore2 - dPreviousLambda * dSlope));

      dParamB = (1 / (dLambda - dPreviousLambda))
                * ((-dPreviousLambda / (pow(dLambda, 2))) * (dPreviousScore - dPreviousScore2 - dLambda * dSlope)
                   + (dLambda / pow(dPreviousLambda, 2)) * (dLastPreviousScore - dPreviousScore2 - dPreviousLambda * dSlope));

      dDisc = dParamB * dParamB - 3 * dParamA * dSlope;

      if (dParamA == 0)
        dLambdaTemp = -dSlope / (2 * dParamB);
      else
        dLambdaTemp = (-dParamB + sqrt(dDisc)) / (3 * dParamA);
    }

    dPreviousLambda    = dLambda;
    dLastPreviousScore = dPreviousScore;
    dLambda            = fmax(0.1 * dLambda, dLambdaTemp);

    for (int i = 0; i < iVectorSize; ++i) {
      pTrialValues[i] = pPreviousCandidates[i] + dLambda * pCholeskyVector[i];
      Candidates[i]   = pTrialValues[i];  // copy back to calling program
    }
    return;
  }

  if (iRet >= 1) {  // bringing a gradient for the quasi-Newton step
    iIters++;

    if (iIters == 1) {
      // Clear Grid and Fill with Approximation.
      for (int i = 0; i < iVectorSize; ++i) {
        for (int j = 0; j < iVectorSize; ++j) pHessianConstruct[i][j] = 0.0;

        if (Score > 1 || Score < -1)
          pHessianConstruct[i][i] = sqrt(fabs(Score));
        else
          pHessianConstruct[i][i] = 1;
      }
    }

    if (iIters > iMaxQuasiSteps) {  // have exceeded maximum no. of iterations
      iRet = -2;
      LOG_MEDIUM() << "FMM: Too many quasi newton iterations  (" << iIters << ")";
      return;
    }

    // Load our Previous Candidates, Last Gradient, and Gradient
    for (int i = 0; i < iVectorSize; i++) {
      pPreviousCandidates[i] = Candidates[i];
      pLastGradient[i]       = pGradient[i];
      pGradient[i]           = Gradient[i];
    }

    dPreviousScore2 = Score;

    // Have We taken the Maximum Newton Step
    if (bMaxStepTaken) {
      iConsecutiveMaxSteps++;

      if (iConsecutiveMaxSteps == 5) {
        LOG_MEDIUM() << "FMM: Max newton steps (5)";
        iRet = -3;
        return;
      }
    } else {
      iConsecutiveMaxSteps = 0;
    }

    // Build our Current Tolerance
    dCurrentTolerance = 0.0;
    for (int i = 0; i < iVectorSize; ++i) {
      dCurrentTolerance = fmax(dCurrentTolerance, fabs(pGradient[i]) * fmax(1, fabs(pPreviousCandidates[i])) / fabs(dPreviousScore2));
    }

    //    if(!(pConfig->getQuietMode())) {
    //      LOG_MEDIUM() << FMM_CONVERGENCE_CHECK << (double)dCurrentTolerance << "\n";
    //      LOG_MEDIUM() << FMM_CONVERGENCE_THRESHOLD << (double)dGradTol << "\n";
    //    }

    if (dCurrentTolerance <= dGradTol) {
      iRet = -1;  // convergence!
      // LOG_MEDIUM() << FMM_CONVERGENCE << dCurrentTolerance;
      // LOG_MEDIUM() << FMM_FUNCTION_SCORE << dPreviousScore2;

      // LOG_MEDIUM() << FMM_CURRENT_PARAMETER_ESTIMATES;
      // for (int i = 0; i < iVectorSize; i++)
      //  LOG_MEDIUM() << pPreviousCandidates[i] << " ";
      // LOG_MEDIUM();

      // LOG_MEDIUM() << FMM_GRADIENT_VALUE;
      // for (int i = 0; i < iVectorSize; i++)
      //  LOG_MEDIUM() << pGradient[i] << " ";
      // LOG_MEDIUM();

      return;
    }

    if (iIters > 1) {
      // step_tol check
      dCurrentTolerance = 0.0;
      for (int i = 0; i < iVectorSize; i++) {
        dCurrentTolerance = fmax(dCurrentTolerance, fabs(pPreviousCandidates[i] - pLastPreviousCandidates[i]) / fmax(fabs(pPreviousCandidates[i]), 1.0));
      }

      if (dCurrentTolerance <= dStepTol) {
        LOG_MEDIUM() << "FMM: Small step size convergence";
        iRet = -1;
        return;
      }

      // and update BFGS Hessian approximation
      bfgsfac(pPreviousCandidates, pLastPreviousCandidates, pGradient, pLastGradient, pHessianConstruct);
    }

    // work out descent direction (using D&S's cholsolve) and put in p
    CholeskyDecomposition(pGradient, pHessianConstruct, pCholeskyVector);
    for (int i = 0; i < iVectorSize; ++i) pCholeskyVector[i] = -pCholeskyVector[i];

    // Reset Some Variables
    iRet               = 0;
    iLinearSearchIters = 0;
    bMaxStepTaken      = false;
    dNewtonStepLength  = 0.0;

    for (int i = 0; i < iVectorSize; ++i) dNewtonStepLength += pCholeskyVector[i] * pCholeskyVector[i];
    dNewtonStepLength = sqrt(dNewtonStepLength);

    if (dNewtonStepLength > dMaxStep) {
      for (int i = 0; i < iVectorSize; i++) pCholeskyVector[i] *= dMaxStep / dNewtonStepLength;

      dNewtonStepLength = (int)dMaxStep;
    }

    dSlope = 0;
    for (int i = 0; i < iVectorSize; i++) dSlope += pGradient[i] * pCholeskyVector[i];

    dRelLength = 0.0;
    for (int i = 0; i < iVectorSize; i++) {
      pTemp[i]   = fabs(pCholeskyVector[i]) / fmax(fabs(pPreviousCandidates[i]), 1);
      dRelLength = fmax(dRelLength, pTemp[i]);
    }

    dLambdaMin = dStepTol * dRelLength;
    dLambda    = 1.0;

    for (int i = 0; i < iVectorSize; i++) {
      pTrialValues[i] = pPreviousCandidates[i] + dLambda * pCholeskyVector[i];
      Candidates[i]   = pTrialValues[i];  // pass back to calling program
    }

    return;
  }
}

//**********************************************************************
// FMM::~FMM()
// Destructor
//**********************************************************************
FMM::~FMM() {
  // Clean up our dynamic memory
  delete[] pPreviousCandidates;
  delete[] pLastPreviousCandidates;
  delete[] pTrialValues;
  delete[] pCholeskyVector;
  delete[] pGradient;
  delete[] pLastGradient;
  delete[] pTemp;

  // Clean the hessian construct memory
  for (int i = 0; i < iVectorSize; ++i) delete[] pHessianConstruct[i];
  delete[] pHessianConstruct;
}

} /* namespace gammadiff */
} /* namespace minimisers */
}  // namespace niwa
