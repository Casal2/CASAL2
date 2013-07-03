/**
 * @file Abundance.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 6/06/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Abundance.h"

// namespaces
namespace isam {
namespace derivedquantities {

/**
 * default constructor
 */
Abundance::Abundance() {
  parameters_.RegisterAllowed(PARAM_TIME_STEP);
  parameters_.RegisterAllowed(PARAM_CATEGORIES);
  parameters_.RegisterAllowed(PARAM_SELECTIVITIES);
  parameters_.RegisterAllowed(PARAM_INITIALIZATION_PHASES);
}

/**
 *
 */
void Abundance::DoValidate() {
  CheckForRequiredParameter(PARAM_TIME_STEP);
  CheckForRequiredParameter(PARAM_CATEGORIES);
  CheckForRequiredParameter(PARAM_SELECTIVITIES);

  time_step_label_                  = parameters_.Get(PARAM_TIME_STEP).GetValue<string>();
  initialisation_time_step_labels_  = parameters_.Get(PARAM_INITIALIZATION_PHASES).GetValues<string>();
  category_labels_                  = parameters_.Get(PARAM_CATEGORIES).GetValues<string>();
  selectivity_labels_               = parameters_.Get(PARAM_SELECTIVITIES).GetValues<string>();

  if (category_labels_.size() != selectivity_labels_.size())
    LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES) << " count (" << selectivity_labels_.size() << ") "
        << " is not the same as the categories count (" << category_labels_.size() << ")");

}


} /* namespace derivedquantities */
} /* namespace isam */
