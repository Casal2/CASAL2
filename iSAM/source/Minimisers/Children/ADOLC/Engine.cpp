/**
 * @file Engine.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef USE_AUTODIFF
#ifdef USE_ADOLC
// headers
#include "Engine.h"

#include <math.h>
#include <iomanip>
#include <adolc/adolc.h>
#include <adolc/taping.h>
#include <adolc/drivers/drivers.h>

#include "FMM.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/Math.h"

// namespaces
namespace niwa {
namespace minimisers {
namespace adolc {

// Namespace
using namespace std;
namespace dc = utilities::doublecompare;
namespace math = utilities::math;

//**********************************************************************
// Engine::Engine()
// Default Constructor
//**********************************************************************
Engine::Engine() {
}

//**********************************************************************
// Engine::~Engine()
// Default Destructor
//**********************************************************************
Engine::~Engine() {
}

//**********************************************************************
// double Engine::optimise_finite_differences(CGammaDiffCallback& objective, vector<double>& StartValues, vector<double>& LowerBounds,
//   vector<double>& UpperBounds, int& convergence, int& iMaxIter, int& iMaxFunc, double dGradTol,
//   double **pOPTIMIZEHessian, int untransformedHessians, double dStepSize)
// OPTIMIZE our function
//**********************************************************************
Double Engine::optimise(adolc::CallBack& objective,
    vector<Double>& start_values, vector<Double>& lower_bounds,
    vector<Double>& upper_bounds, int& convergence, int& max_iterations,
    int& max_evaluations, Double gradient_tolerance, double **out_hessian,
    int untransformed_hessians, Double step_size) {

  // Variables
  unsigned parameter_count = start_values.size();
  double obj_score = 0.0;
  adouble aobj_score = 0.0;
  Double penalty = 0.0;
  vector<double> scaled_candidate_values(parameter_count, 0.0);
  vector<double> gradient_values(parameter_count, 0.0);

//  bool first_iteration = true;

  /**
   * Validate our values, bounds etc
   */
  if (lower_bounds.size() != parameter_count)
    LOG_CODE_ERROR() << "lower_bounds.size() != parameter_count";
  if (upper_bounds.size() != parameter_count)
    LOG_CODE_ERROR() << "upper_bounds.size() != parameter_count";

  // Create our Minimiser
  FMM fmm(parameter_count, max_evaluations, max_iterations, gradient_tolerance.value());

  vector<adouble> candidates(parameter_count, 0.0);
  vector<adouble> scaled_candidates(parameter_count, 0.0);

  /**
   * Scale our start values
   */
  LOG_MEDIUM() << "Start Values (scaled): ";
  for (unsigned i = 0; i < start_values.size(); ++i) {
    if (start_values[i] < lower_bounds[i])
      LOG_CODE_ERROR() << "start_values[i] < lower_bounds[i]";
    if (start_values[i] > upper_bounds[i])
      LOG_CODE_ERROR() << "start_values[i] > upper_bounds[i]";


    if (dc::IsEqual(lower_bounds[i], upper_bounds[i]))
      scaled_candidates[i] = 0.0;
    else
      scaled_candidates[i] = math::scale_value(start_values[i], lower_bounds[i], upper_bounds[i]);
    LOG_MEDIUM() << scaled_candidates[i] << ", ";
  }
  LOG_MEDIUM() << endl;

  // Loop through our Minimiser now
  while (fmm.getResult() >= 0) {
    // Do we need to evaluate objective function again?
    if ((fmm.getResult() == 0) || (fmm.getResult() == 2)) {
      LOG_MEDIUM() << "About to trace the objective (model)" << endl;
//      if (first_iteration) // only run once
        trace_on(0);

      // declare our dependent variables
      for (unsigned i = 0; i < candidates.size(); ++i)
        scaled_candidates[i] <<= scaled_candidates[i].value();

      // Reset Variables
      penalty = 0.0;

      // unscale candidates
      LOG_MEDIUM() << "candidates (unscaled): ";
      for (unsigned i = 0; i < parameter_count; ++i) {
        if (dc::IsEqual(lower_bounds[i], upper_bounds[i]))
          candidates[i] = lower_bounds[i];
        else
          candidates[i] = math::unscale_value(scaled_candidates[i], penalty, lower_bounds[i], upper_bounds[i]);
        LOG_MEDIUM() << candidates[i] << ", ";
      }
      LOG_MEDIUM() << endl;

      LOG_MEDIUM() << "Running Model: Start -->";
      aobj_score = objective(candidates);
      LOG_MEDIUM() << " End" << endl;
      aobj_score += penalty; // penalty for breaking bounds
      aobj_score >>= obj_score;
//      if (first_iteration) { // only run once
        trace_off();
//        first_iteration = false;
//      }
      LOG_MEDIUM() << "Finished objective function call with score = " << obj_score << " (inc Penalty: " << penalty << ")" << endl;
    }

    // Gradient Required
    // This will loop through each variable changing it once
    // to see how the other variables change.
    // There-by generating our co-variance
    if (fmm.getResult() >= 1) { // 1 = Gradient Required
      LOG_MEDIUM() << "About to eval gradient" << endl;

      double* adolc_x = new double[parameter_count];
      double* adolc_g = new double[parameter_count];

      LOG_MEDIUM() << "adolc_x: ";
      for (unsigned i = 0; i < parameter_count; ++i) {
        adolc_x[i] = scaled_candidates[i].value();
        LOG_MEDIUM() << adolc_x[i] << ", ";
      }
      LOG_MEDIUM() << endl;

      int g_status = gradient(0, parameter_count, adolc_x, adolc_g);
//      double one = 1.0;
//      int g_status = fos_reverse(0, 1, parameter_count, &one, adolc_g);
      LOG_MEDIUM() << "Finished gradient call with status: " << g_status << endl;

      LOG_MEDIUM() << "gradient: ";
      for (unsigned i = 0; i < parameter_count; ++i) {
        gradient_values[i] = adolc_g[i];
        LOG_MEDIUM() << gradient_values[i] << ", ";
      }
      LOG_MEDIUM() << endl;

      delete [] adolc_x;
      delete [] adolc_g;
      // Gradient Finished
    }
    /**
     * Scale our candidates in to candidate_values so we can
     * pass them to fmin() which will build use a new set of
     * scaled candidates.
     *
     * Then we assign them back so they can be unscaled during
     * the trace
     */
    LOG_MEDIUM() << "before: ";
    for (unsigned i = 0; i < candidates.size(); ++i) {
      scaled_candidate_values[i] = scaled_candidates[i].value();
      LOG_MEDIUM() << scaled_candidate_values[i] << ",";
    }
    LOG_MEDIUM() << endl;

    fmm.fMin(scaled_candidate_values, obj_score, gradient_values);

    LOG_MEDIUM() << "after: ";
    for (unsigned i = 0; i < candidates.size(); ++i) {
      scaled_candidates[i] = scaled_candidate_values[i];
      LOG_MEDIUM() << scaled_candidates[i] << ", ";
    }
    LOG_MEDIUM() << endl;
    LOG_MEDIUM() << "FMM score: " << obj_score << endl;
    LOG_MEDIUM() << "FMM Result: " << fmm.getResult() << endl;
  }

  if (fmm.getResult() == -3)
    LOG_MEDIUM() << "Convergence Unclear" << endl;
  else if (fmm.getResult() == -2)
    LOG_MEDIUM() << "Max Evaluations" << endl;
  else if (fmm.getResult() == -1)
    LOG_MEDIUM() << "BAM CONVERGENCE" << endl;
  else
    LOG_MEDIUM() << "UNKNOWN RETURN VALUE: " << fmm.getResult() << endl;

  /**
   * Unscale our values
   */
//  for (unsigned i = 0; i < candidates.size(); ++i)
//    candidates[i] = math::unscale_value(candidates[i], penalty, lower_bounds[i], upper_bounds[i]).value();

  adouble final_score = objective(candidates);

  // Generate our Hessian
  if (out_hessian != 0) {
    double **L = new double*[parameter_count];
    double **LT = new double*[parameter_count];

    for (unsigned i = 0; i < parameter_count; ++i) {
      L[i] = new double[parameter_count];
      LT[i] = new double[parameter_count];
    }

    for (unsigned i = 0; i < parameter_count; ++i) {
      for (unsigned j = 0; j < parameter_count; ++j) {
        L[i][j] = 0.0;
        LT[i][j] = 0.0;
      }
    }

    for (unsigned i = 0; i < parameter_count; ++i) {
      for (unsigned j = 0; j <= i; ++j) {
        L[i][j] = LT[j][i] = fmm.getHessianValue(i, j);
      }
    }

    for (unsigned i = 0; i < parameter_count; ++i) {
      for (unsigned j = 0; j < parameter_count; ++j) {
        double dMulti = 0.0;

        // Loop Through
        for (unsigned k = 0; k < parameter_count; ++k)
          dMulti += (L[i][k] * LT[k][j]);

        out_hessian[i][j] = dMulti;
      }
    }

    if (untransformed_hessians) {
      double *dGradBoundP = new double[parameter_count];
      for (unsigned i = 0; i < parameter_count; ++i)
        dGradBoundP[i] = 0.0;

      for (unsigned i = 0; i < parameter_count; ++i) {
        double dDiv = ((candidates[i].value() - lower_bounds[i].value())
            / (upper_bounds[i].value() - lower_bounds[i].value()));
        double dProd = (2 * dDiv - 1) * (2 * dDiv - 1);
        double dSqrt = sqrt(dc::ZeroFun(1 - dProd));
        double dProd2 = (upper_bounds[i].value() - lower_bounds[i].value()) * dSqrt;
        dGradBoundP[i] = ((4 / 3.14159265) / dProd2);
      }

      for (unsigned i = 0; i < parameter_count; ++i) {
        if (dGradBoundP[i] != dGradBoundP[i]) // NaN
          dGradBoundP[i] = 0.0;
      }

      for (unsigned i = 0; i < parameter_count; ++i)
        for (unsigned j = 0; j < parameter_count; ++j) {
          out_hessian[i][j] *= dGradBoundP[i];
          out_hessian[j][i] *= dGradBoundP[i];
        }

      delete[] dGradBoundP;
    }

    for (unsigned i = 0; i < parameter_count; ++i) {
      delete[] L[i];
      delete[] LT[i];
    }

    delete[] L;
    delete[] LT;
  }

  LOG_MEDIUM() << "final: ";
  for (unsigned i = 0; i < candidates.size(); ++i) {
    LOG_MEDIUM() << candidates[i] << ", ";
  }
  LOG_MEDIUM() << endl;
  for (adouble value : candidates)
    final_candidates_.push_back(value);

  convergence_     = fmm.getResult() + 2;
  iterations_used_ = fmm.getIters();
  evaluations_used_ = fmm.getEvals();

  return final_score;
}

} /* namespace adolc */
} /* namespace minimisers */
} /* namesapce isam */
#endif /* USE_ADOLC */
#endif /* USE_AUTODIFF */
