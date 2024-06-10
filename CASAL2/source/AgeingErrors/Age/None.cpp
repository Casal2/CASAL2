/**
 * @file none.cpp
 * @author C Marsh
 * @github https://github.com/Zaita
 * @date 11/08/2016
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "None.h"

#include "../../Utilities/To.h"

// namespaces
namespace niwa {
namespace ageingerrors {

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g., profiling, yields, projections, etc.)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate LaTeX for the documentation.
 */
None::None(shared_ptr<Model> model) : AgeingError(model) {}

/**
 * Destructor
 */
None::~None() {}

/**
 * Reset the diagonal of the misspecification matrix to 1
 */
void None::DoBuild() {
  unsigned j = 0;
  // No ageing error so we create a misspecification matrix of 1's on the diagonal.
  for (unsigned i = 0; i < age_spread_; ++i, ++j) mis_matrix_[i][j] = 1.0;
}

} /* namespace ageingerrors */
} /* namespace niwa */
