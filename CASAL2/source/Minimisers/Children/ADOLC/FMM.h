/**
 * @file FMM.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 17/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef USE_AUTODIFF
#ifdef USE_ADOLC
#ifndef MINIMISERS_ADOLC_FMM_H_
#define MINIMISERS_ADOLC_FMM_H_

#include <vector>

// namespaces
namespace niwa {
namespace minimisers {
namespace adolc {

using std::vector;

/**
 * Class Definition
 */
class FMM {
public:
  // Functions
  FMM(int vectorSize, int maxFunc, int maxQuasiSteps, double gradTol);
  virtual                     ~FMM();
  void                        fMin(vector<double>& Candidates, double& Score, vector<double>& Gradient);
  int                         getResult() { return iRet; }
  double                      getHessianValue(int i, int j) { return pHessianConstruct[i][j]; }
  int                         getIters() { return iIters; }
  int                         getEvals() { return iEvals; }

private:
  // Functions
  void bfgsfac(long double* x,long double* xlast,long double* g,long double* glast,long double **pHessianConstruct);
  void CholeskyDecomposition(long double* Gradient, long double** LowerTriangle, long double* CholeskyVector);
  void qrupdate(long double* u,long double* v,long double** M);
  void jacrotate(int i,long double a,long double b,long double** M);

  // Variables
  int                         iMaxFunc;         // Max Function Evals
  int                         iMaxQuasiSteps;   // Max Quasi-Newton steps
  int                         iMaxSteps;        // maximum no. of steps in a single linear search
  long double                 dGradTol;         // scaled gradient (see Dennis & Schnabel p 279) required for termination
  long double                 dStepTol;         // scaled step size (D&S p 279) required for termination
  long double                 dMaxStep;         // maximum scaled step size (D&S p 279) permitted
  int                         iIters;
  int                         iEvals;           // quasi-Newton iterations, function evaluations
  int                         iRet;
  int                         iVectorSize;
  long double                 *pPreviousCandidates;
  long double                 *pLastPreviousCandidates;
  long double                 *pTrialValues;
  long double                 dPreviousScore;
  long double                 dPreviousScore2;
  long double                 dLastPreviousScore;
  int                         iLinearSearchIters;
  long double                 **pHessianConstruct;  // Hessian Construct. Used to build Hessian
  long double                 dAlpha;
  long double                 dLambda;
  long double                 dLambdaMin;
  long double                 dLambdaTemp;
  long double                 dPreviousLambda;
  long double                 dSlope;
  long double                 dParamA;
  long double                 dParamB;
  bool                        bMaxStepTaken;
  int                         iConsecutiveMaxSteps;
  long double                 dNewtonStepLength;
  long double                 *pCholeskyVector;
  long double                 *pGradient;
  long double                 *pLastGradient;
  long double                 dCurrentTolerance;
  double                      dEpsilon;
  long double                 *pTemp;
  long double                 dRelLength;
  long double                 dDisc;
  //rellength,disc;
};

} /* namespace adolc */
} /* namespace minimisers */
} /* namesapce niwa */

#endif /* MINIMISERS_ADOLC_FMM_H_ */
#endif /* USE_ADOLC */
#endif /* USE_AUTODIFF */
