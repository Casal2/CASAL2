
/**
 * @file Engine.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef USE_AUTODIFF
// Headers
#include "Engine.h"

#include <memory>
#include <iostream>
#include <cmath>

#include "../../../Utilities/Math.h"
#include "../../../Utilities/RandomNumberGenerator.h"
#include "../../../Logging/Logging.h"

// Namespaces
namespace niwa {
namespace minimisers {
namespace desolver {

namespace math = niwa::utilities::math;

/**
 * Default constructor
 *
 * @param vector_size The size of our vectors with candidate bounds
 * @param population_size The starting population size
 * @param tolerance The tolerance threshold before convergance
 */
Engine::Engine(unsigned vector_size, unsigned population_size, double tolerance) {
  vector_size_      = vector_size;
  population_size_  = population_size;
  generations_      = 0;
  scale_            = 0.7;
  probability_      = 0.5;
  best_energy_      = 1e20;
  step_size_        = 1e-6;
  tolerance_        = tolerance;
  penalty_          = 0.0;

  // Resize vectors
  current_values_.resize(vector_size_);
  scaled_values_.resize(vector_size_);
  lower_bounds_.resize(vector_size_);
  upper_bounds_.resize(vector_size_);
  best_solution_.resize(vector_size_);
  gradient_values_.resize(vector_size_);
  population_energy_.resize(population_size_);

  best_solution_.assign(vector_size_, 0.0);

  for (unsigned i = 0; i < population_size_; ++i)
    population_[i].resize(vector_size_);
}

/**
 * Destructor
 */
Engine::~Engine() {
}

/**
 * Set up the variables for the DE Solver engine
 *
 * @param start_values The starting values for all candidates
 * @param lower_bounds The lower bounds for all candidates
 * @param upper_bounds The upper bounds for all candidates
 * @param de_strategy The method of evolution to use
 * @param diff_scale The difference scale to use when stepping
 * @param crossover_prob The crossover probability
 */
void Engine::Setup(vector<double> start_values, vector<double> lower_bounds,
                                vector<double> upper_bounds, int de_strategy, double diff_scale,
                                double crossover_prob) {
  switch (de_strategy) {
    case kBest1Exp:
      calculate_solution_ = &Engine::Best1Exp;
      number_of_parents_  = 2;
      break;

    case kRand1Exp:
      calculate_solution_ = &Engine::Rand1Exp;
      number_of_parents_  = 3;
      break;

    case kRandToBest1Exp:
      calculate_solution_ = &Engine::RandToBest1Exp;
      number_of_parents_  = 2;
      break;

    case kBest2Exp:
      calculate_solution_ = &Engine::Best2Exp;
      number_of_parents_  = 4;
      break;

    case kRand2Exp:
      calculate_solution_ = &Engine::Rand2Exp;
      number_of_parents_  = 5;
      break;

    case kBest1Bin:
      calculate_solution_ = &Engine::Best1Bin;
      number_of_parents_  = 2;
      break;

    case kRand1Bin:
      calculate_solution_ = &Engine::Rand1Bin;
      number_of_parents_  = 3;
    break;

    case kRandToBest1Bin:
      calculate_solution_ = &Engine::RandToBest1Bin;
      number_of_parents_  = 2;
      break;

    case kBest2Bin:
      calculate_solution_ = &Engine::Best2Bin;
      number_of_parents_  = 4;
      break;

    case kRand2Bin:
      calculate_solution_ = &Engine::Rand2Bin;
      number_of_parents_  = 5;
      break;

    default:
      break;
  }

  scale_        = diff_scale;
  probability_  = crossover_prob;

  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  for (unsigned i = 0; i < population_size_; ++i) {
    for (unsigned j = 0; j < vector_size_; ++j)
      population_[i][j] = rng.uniform(lower_bounds[j], upper_bounds[j]);

    population_energy_[i] = 1e20;
  }

  current_values_.assign(start_values.begin(), start_values.end());
  best_solution_.assign(start_values.begin(), start_values.end());
  lower_bounds_.assign(lower_bounds.begin(), lower_bounds.end());
  upper_bounds_.assign(upper_bounds.begin(), upper_bounds.end());
}

/**
 * Start solving the model
 *
 * @param max_generations
 * @return True if we solve, false otherwise
 */
bool Engine::Solve(unsigned max_generations) {
  bool new_best_energy  = false;

  trial_energy_ = EnergyFunction(current_values_);
  LOG_MEDIUM() << "First Trial Energy: " << trial_energy_;
  if (trial_energy_ < best_energy_) {
    new_best_energy = true;

    // Copy the solution to our best.
    best_energy_    = trial_energy_;
    best_solution_  = current_values_;

    LOG_MEDIUM() << "Current estimates: ";
    for (unsigned i = 0; i < best_solution_.size(); ++i)
      LOG_MEDIUM() << best_solution_[i] << " ";
    LOG_MEDIUM();
    LOG_MEDIUM() << "Objective function value: " << trial_energy_;
  }

  for (unsigned i = 0; i < max_generations; ++i) {
    LOG_MEDIUM() << "DESolver: current generation: " << (i+1) << "\n";
    for (unsigned j = 0; j < population_size_; ++j) {
      // Build our Trial Solution
      (this->*calculate_solution_)(j);

      trial_energy_ = EnergyFunction(current_values_);
      if (trial_energy_ < population_energy_[j]) {
        // Copy solution to our Population
        population_energy_[j] = trial_energy_;
        population_[j].assign(current_values_.begin(), current_values_.end());

        // Is this a new all-time low for our search?
        if (trial_energy_ < best_energy_) {
          new_best_energy = true;
          // Copy the solution to our best.
          best_energy_ = trial_energy_;
          best_solution_.assign(current_values_.begin(), current_values_.end());


          LOG_MEDIUM() << "Objective function value: " << trial_energy_;

//          if(!(pConfig->getQuietMode())) {
//            LOG_MEDIUM() << "Current estimates: ";
//            for (int k = 0; k < (int)vBestSolution.size(); ++k)
//              LOG_MEDIUM() << vBestSolution[k] << " ";
//            LOG_MEDIUM();
//            LOG_MEDIUM() << "Objective function value: " << dTrialEnergy << "\n";
//          }
        }
      }
    } // end for()

    // If we have a new Best, lets generate a gradient.
    if (new_best_energy)
      if (GenerateGradient()) {
        generations_ = i;
        return true; // Convergence!
      }

    new_best_energy = false;
  }

  LOG_MEDIUM() << "Best Solution: ";
  for (unsigned i = 0; i < best_solution_.size(); ++i)
    LOG_MEDIUM() << best_solution_[i] << " ";
  LOG_MEDIUM() << "= " << best_energy_;

  LOG_MEDIUM() << "Population:";
  for (unsigned i = 0; i < population_size_; ++i) {
    LOG_MEDIUM() << i << "] ";
    for (unsigned j = 0; j < vector_size_; ++j) {
      LOG_MEDIUM() << population_[i][j] << " ";
    }
    LOG_MEDIUM() << "= " << population_energy_[i];
  }

  return false;
}

/**
 *
 */
bool Engine::GenerateGradient() {

  double convergence_check = 0;
  for (unsigned i = 0; i < vector_size_; ++i) {
    // Create Vars
    double min = 1e20;
    double max = -1e20;

    for (unsigned j = 0; j < population_size_; ++j) {
      double scaled = ScaleValue(population_[j][i], lower_bounds_[i], upper_bounds_[i]);

      if (scaled < min)
        min = scaled;
      if (scaled > max)
        max = scaled;
    }

    convergence_check = max - min;


    if (convergence_check > tolerance_) {
      LOG_MEDIUM() << "DESolver: (no convergence) convergence_check: " << convergence_check << "; tolerance: " << tolerance_;
      return false; // No Convergence
    }
  }

  LOG_MEDIUM() << "DESolver: (convergence) convergence_check: " << convergence_check << "; tolerance: " << tolerance_;
  return true; // Convergence
}

/**
 * Scale the current values so they're between -1.0 and 1.0
 */
void Engine::ScaleValues() {
  for (unsigned i = 0; i < vector_size_; ++i) {
    // Boundary-Pinning
    if (math::IsEqual(lower_bounds_[i], upper_bounds_[i]))
      scaled_values_[i] = 0.0;
    else
      scaled_values_[i] = ScaleValue(current_values_[i], lower_bounds_[i], upper_bounds_[i]);
  }
}

/**
 * Unscale the current values back to their original values
 */
void Engine::UnScaleValues() {
  for (unsigned i = 0; i < vector_size_; ++i) {
    if (math::IsEqual(lower_bounds_[i], upper_bounds_[i]))
      current_values_[i] = lower_bounds_[i];
    else
      current_values_[i] = UnScaleValue(scaled_values_[i], lower_bounds_[i], upper_bounds_[i]);
  }
}

/**
 * Scale a value so that it's between -1.0 and 1.0 using the
 * bounds as a reference.
 *
 * @param value The value to scale
 * @param min lower bound
 * @param max upper bound
 * @return The scaled value
 */
double Engine::ScaleValue(double value, double min, double max) {
  if (math::IsEqual(value, min))
    return -1;
  else if (math::IsEqual(value, max))
    return 1;

  return asin(2 * (value - min) / (max - min) - 1) / 1.57079633;
}

/**
 * Unscale a value back from -1.0 and 1.0
 *
 * @param value The value to scale
 * @param min The lower bound
 * @param max The upper bound
 * @return The unscaled value
 */
double Engine::UnScaleValue(const double& value, double min, double max) {
  // courtesy of AUTODIF - modified to correct error -
  // penalty on values outside [-1,1] multiplied by 100 as of 14/1/02.
  double t = 0.0;
  double y = 0.0;

  t = min + (max - min) * (sin(value * 1.57079633) + 1) / 2;
  this->CondAssign(y, -.9999 - value, (value + .9999) * (value + .9999), 0);
  penalty_ += y;
  this->CondAssign(y, value - .9999, (value - .9999) * (value - .9999), 0);
  penalty_ += y;
  this->CondAssign(y, -1 - value, 1e5 * (value + 1) * (value + 1), 0);
  penalty_ += y;
  this->CondAssign(y, value - 1, 1e5 * (value - 1) * (value - 1), 0);
  penalty_ += y;

  return (t);
}

/**
 * Conditional Assignment
 */
void Engine::CondAssign(double &res, const double &cond, const double &arg1, const double &arg2) {
  res = (cond) > 0 ? arg1 : arg2;
}

/**
 * Conditional Assignment
 */
void Engine::CondAssign(double &res, const double &cond, const double &arg) {
  res = (cond) > 0 ? arg : res;
}

/**
 * Select some population indexes to use for the next candidate
 */
void Engine::SelectSamples(unsigned candidate) {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  double population_size = population_size_ * 1.0;

  // Build first Sample
  if (number_of_parents_ >= 1) {
    do {
      r1_ = rng.uniform(0.0, population_size);
    } while (r1_ == candidate);
  } else
    return;

  // Build Second Sample
  if (number_of_parents_ >= 2) {
    do {
      r2_ = (int) rng.uniform(0.0, population_size);
    } while ((r2_ == candidate) || (r2_ == r1_));
  } else
    return;

  // Build third sample
  if (number_of_parents_ >= 3) {
    do {
      r3_ = rng.uniform(0.0, population_size);
    } while ((r3_ == candidate) || (r3_ == r2_) || (r3_ == r1_));
  } else
    return;

  // etc
  if (number_of_parents_ >= 4) {
    do {
      r4_ = rng.uniform(0.0, population_size);
    } while ((r4_ == candidate) || (r4_ == r3_) || (r4_ == r2_) || (r4_ == r1_));
  }

  // etc
  if (number_of_parents_ >= 5) {
    do {
      r5_ = rng.uniform(0.0, population_size);
    } while ((r5_ == candidate) || (r5_ == r4_) || (r5_ == r3_) || (r5_ == r2_) || (r5_ == r1_));
  }

  return;
}

//**********************************************************************
// void DESolverEngine::Best1Exp(int candidate)
// Generate A Solution from our Best Score
//**********************************************************************
void Engine::Best1Exp(unsigned candidate) {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  // Select our Previous Generations to Sample From
  SelectSamples(candidate);

  // Build our Current values from Target Generation
  current_values_.assign(population_[candidate].begin(), population_[candidate].end());

  // Generate new values for our Current by using probability and scale and then
  // making a slight adjustment to the vBestSolution.
  for (unsigned i = 0; i < vector_size_; ++i) {
    if (rng.uniform() < probability_) {
      current_values_[i] = best_solution_[i] + (scale_ * (population_[r1_][i] - population_[r2_][i]));

      if (current_values_[i] < lower_bounds_[i])
        current_values_[i] = lower_bounds_[i];
      if (current_values_[i] > upper_bounds_[i])
        current_values_[i] = upper_bounds_[i];
    }
  }
}

//**********************************************************************
//
//
//**********************************************************************
void Engine::Rand1Exp(unsigned candidate) {
  /*int r1, r2, r3;
  int n;

  SelectSamples(candidate, &r1, &r2, &r3);
  n = (int) pRandom -> getRandomUniform(0.0, (double) nDim);

  CopyVector(trialSolution,RowVector(population,candidate));
  for (int i = 0; (pRandom -> getRandomUniform_01() < probability) && (i < nDim); i++) {
    trialSolution[n] = Element(population,r1,n) + scale * (Element(population,r2,n) - Element(population,r3,n));
    n = (n + 1) % nDim;
  }*/

  return;
}

//**********************************************************************
//
//
//**********************************************************************
void Engine::RandToBest1Exp(unsigned candidate) {
  /*int r1, r2;
  int n;

  SelectSamples(candidate, &r1, &r2);
  n = (int) pRandom -> getRandomUniform(0.0, (double) nDim);

  CopyVector(trialSolution,RowVector(population,candidate));
  for (int i = 0; (pRandom -> getRandomUniform_01() < probability) && (i < nDim); i++) {
    trialSolution[n] += scale * (bestSolution[n] - trialSolution[n]) + scale * (Element(population,r1,n) - Element(population,r2,n));
    n = (n + 1) % nDim;
  }
*/
  return;
}

//**********************************************************************
//
//
//**********************************************************************
void Engine::Best2Exp(unsigned candidate) {
  /*int r1, r2, r3, r4;
  int n;

  SelectSamples(candidate, &r1, &r2, &r3, &r4);
  n = (int) pRandom -> getRandomUniform(0.0, (double) nDim);

  CopyVector(trialSolution,RowVector(population,candidate));
  for (int i = 0; (pRandom -> getRandomUniform_01() < probability) && (i < nDim); i++) {
    trialSolution[n] = bestSolution[n] + scale * (Element(population,r1,n) + Element(population,r2,n) - Element(population,r3,n) - Element(population,r4,n));
    n = (n + 1) % nDim;
  }
*/
  return;
}

//**********************************************************************
//
//
//**********************************************************************
void Engine::Rand2Exp(unsigned candidate) {
  /*int r1, r2, r3, r4, r5;
  int n;

  SelectSamples(candidate, &r1, &r2, &r3, &r4, &r5);
  n = (int) pRandom -> getRandomUniform(0.0, (double) nDim);

  CopyVector(trialSolution,RowVector(population,candidate));
  for (int i = 0; (pRandom -> getRandomUniform_01() < probability) && (i < nDim); i++) {
    trialSolution[n] = Element(population,r1,n) + scale * (Element(population,r2,n) + Element(population,r3,n) - Element(population,r4,n) - Element(population,r5,n));
    n = (n + 1) % nDim;
  }*/

  return;
}

//**********************************************************************
//
//
//**********************************************************************
void Engine::Best1Bin(unsigned candidate) {
  /*int r1, r2;
  int n;

  SelectSamples(candidate, &r1, &r2);
  n = (int) pRandom -> getRandomUniform(0.0, (double) nDim);

  CopyVector(trialSolution,RowVector(population,candidate));
  for (int i = 0; i < nDim; i++) {
    if ((pRandom -> getRandomUniform_01() < probability) || (i == (nDim - 1)))
      trialSolution[n] = bestSolution[n] + scale * (Element(population,r1,n) - Element(population,r2,n));
    n = (n + 1) % nDim;
  }
*/
  return;
}

//**********************************************************************
//
//
//**********************************************************************
void Engine::Rand1Bin(unsigned candidate) {
  /*int r1, r2, r3;
  int n;

  SelectSamples(candidate, &r1, &r2, &r3);
  n = (int) pRandom -> getRandomUniform(0.0, (double) nDim);

  CopyVector(trialSolution,RowVector(population,candidate));
  for (int i = 0; i < nDim; i++) {
    if ((pRandom -> getRandomUniform_01() < probability) || (i == (nDim - 1)))
      trialSolution[n] = Element(population,r1,n) + scale * (Element(population,r2,n) - Element(population,r3,n));
    n = (n + 1) % nDim;
  }*/

  return;
}

//**********************************************************************
//
//
//**********************************************************************
void Engine::RandToBest1Bin(unsigned candidate) {
  /*int r1, r2;
  int n;

  SelectSamples(candidate, &r1, &r2);
  n = (int) pRandom -> getRandomUniform(0.0, (double) nDim);

  CopyVector(trialSolution,RowVector(population,candidate));
  for (int i = 0; i < nDim; i++) {
    if ((pRandom -> getRandomUniform_01() < probability) || (i == (nDim - 1)))
      trialSolution[n] += scale * (bestSolution[n] - trialSolution[n]) + scale * (Element(population,r1,n) - Element(population,r2,n));
    n = (n + 1) % nDim;
  }
*/
  return;
}

//**********************************************************************
//
//
//**********************************************************************
void Engine::Best2Bin(unsigned candidate) {
  /*int r1, r2, r3, r4;
  int n;

  SelectSamples(candidate, &r1, &r2, &r3, &r4);
  n = (int) pRandom -> getRandomUniform(0.0, (double) nDim);

  CopyVector(trialSolution,RowVector(population,candidate));
  for (int i = 0; i < nDim; i++) {
    if ((pRandom -> getRandomUniform_01() < probability) || (i == (nDim - 1)))
      trialSolution[n] = bestSolution[n] + scale * (Element(population,r1,n) + Element(population,r2,n) - Element(population,r3,n) - Element(population,r4,n));
    n = (n + 1) % nDim;
  }
*/
  return;
}

//**********************************************************************
//
//
//**********************************************************************
void Engine::Rand2Bin(unsigned candidate) {
  /*int r1, r2, r3, r4, r5;
  int n;

  SelectSamples(candidate, &r1, &r2, &r3, &r4, &r5);
  n = (int) pRandom -> getRandomUniform(0.0, (double) nDim);

  CopyVector(trialSolution,RowVector(population,candidate));
  for (int i = 0; i < nDim; i++) {
    if ((pRandom -> getRandomUniform() < probability) || (i == (nDim - 1)))
      trialSolution[n] = Element(population,r1,n) + scale * (Element(population,r2,n) + Element(population,r3,n) - Element(population,r4,n) - Element(population,r5,n));
    n = (n + 1) % nDim;
  }
*/
  return;
}

} /* namespace desolver */
} /* namespace minimisers */
} /* namespace niwa */
#endif
