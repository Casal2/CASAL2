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

// Headers
#include "Engine.h"

#include <memory>
#include <iostream>
#include <cmath>

#include "Utilities/RandomNumberGenerator.h"

// Namespaces
namespace isam {
namespace minimisers {
namespace desolver {

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
 *
 */
void Engine::SelectSamples(unsigned candidate) {

  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  // Build first Sample
  if (number_of_parents_ >= 1) {
    do {
      r1_ = rng.uniform(0.0, (double) population_size_);
    } while (r1_ == candidate);
  } else
    return;

  // Build Second Sample
  if (number_of_parents_ >= 2) {
    do {
      r2_ = (int) rng.uniform(0.0, (double) population_size_);
    } while ((r2_ == candidate) || (r2_ == r1_));
  } else
    return;

  // Build third sample
  if (number_of_parents_ >= 3) {
    do {
      r3_ = rng.uniform(0.0, (double) population_size_);
    } while ((r3_ == candidate) || (r3_ == r2_) || (r3_ == r1_));
  } else
    return;

  // etc
  if (number_of_parents_ >= 4) {
    do {
      r4_ = rng.uniform(0.0, (double) population_size_);
    } while ((r4_ == candidate) || (r4_ == r3_) || (r4_ == r2_) || (r4_ == r1_));
  }

  // etc
  if (number_of_parents_ >= 5) {
    do {
      r5_ = rng.uniform(0.0, (double) population_size_);
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
} /* namespace isam */
