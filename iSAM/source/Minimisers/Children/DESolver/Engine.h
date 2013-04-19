/**
 * @file Engine.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Based on algorithms developed by Dr. Rainer Storn & Kenneth Price
 * written By: Lester E. Godwin
 *              PushCorp, Inc.
 *              Dallas, Texas
 *              972-840-0208 x102
 *              godwin@pushcorp.com
 * Created: 6/8/98
 * Last Modified: 6/8/98
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef MINIMISERS_DESOLVER_ENGINE_H_
#define MINIMISERS_DESOLVER_ENGINE_H_

// Headers
#include <map>
#include <vector>

// Namespaces
namespace isam {
namespace minimisers {
namespace desolver {

using std::map;
using std::vector;

// Defines
#define kBest1Exp         0
#define kRand1Exp         1
#define kRandToBest1Exp   2
#define kBest2Exp         3
#define kRand2Exp         4
#define kBest1Bin         5
#define kRand1Bin         6
#define kRandToBest1Bin   7
#define kBest2Bin         8
#define kRand2Bin         9

class Engine;
typedef void (minimisers::desolver::Engine::*StrategyFunction)(unsigned);

/**
 * Class Definition
 */
class Engine {
public:
  Engine(unsigned vector_size, unsigned population_size, double tolerance);
  virtual                     ~Engine();
  void                        Setup(vector<double> start_values, vector<double> lower_bounds,
                                  vector<double> upper_bounds, int de_strategy, double diff_scale,
                                  double crossover_prob);
  virtual bool                Solve(unsigned max_generations);
  virtual double              EnergyFunction(vector<double> test_solution) = 0;

private:
  // Methods
  void                        SelectSamples(unsigned candidate);
  bool                        GenerateGradient();
  void                        ScaleValues();
  void                        UnScaleValues();
  double                      ScaleValue(double value, double min, double max);
  double                      UnScaleValue(const double& value, double min, double max);
  void                        CondAssign(double &res, const double &cond, const double &arg1, const double &arg2);
  void                        CondAssign(double &res, const double &cond, const double &arg);
  void                        Best1Exp(unsigned candidate);
  void                        Rand1Exp(unsigned candidate);
  void                        RandToBest1Exp(unsigned candidate);
  void                        Best2Exp(unsigned candidate);
  void                        Rand2Exp(unsigned candidate);
  void                        Best1Bin(unsigned candidate);
  void                        Rand1Bin(unsigned candidate);
  void                        RandToBest1Bin(unsigned candidate);
  void                        Best2Bin(unsigned candidate);
  void                        Rand2Bin(unsigned candidate);

  // Members
  unsigned                    vector_size_;
  vector<double>              current_values_;
  vector<double>              scaled_values_;
  vector<double>              lower_bounds_;
  vector<double>              upper_bounds_;
  map<unsigned, vector<double> > population_;
  vector<double>              population_energy_;
  vector<double>              best_solution_;
  vector<double>              gradient_values_;
  unsigned                    population_size_;
  unsigned                    number_of_parents_;
  unsigned                    generations_;
  double                      scale_;
  double                      probability_;
  double                      best_energy_;
  double                      trial_energy_;
  StrategyFunction            calculate_solution_;
  unsigned                    r1_;
  unsigned                    r2_;
  unsigned                    r3_;
  unsigned                    r4_;
  unsigned                    r5_;
  double                      step_size_;
  double                      penalty_;
  double                      tolerance_;
};

} /* namespace desolver */
} /* namespace minimisers */
} /* namespace isam */
#endif /* MINIMISERS_DESOLVER_ENGINE_H_ */
