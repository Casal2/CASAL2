/**
 * @file Ageing.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Ageing.h"

#include "Utilities/To.h"
#include "Categories/Categories.h"

// Namespaces
namespace isam {
namespace processes {

/**
 * Default constructor
 */
Ageing::Ageing() {
  LOG_TRACE();

  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_CATEGORIES);
}

/**
 * Validate our ageing process.
 *
 * 1. Check for any required parameters
 * 2. Assign the label from our parameters
 * 3. Assign any remaining parameters
 */
void Ageing::Validate() {
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_CATEGORIES);

  label_          = parameters_.Get(PARAM_LABEL).GetValue<string>();
  category_names_ = parameters_.Get(PARAM_CATEGORIES).GetValues<string>();

  // Ensure defined categories were valid
  for(const string& category : category_names_) {
    if (!Categories::Instance()->IsValid(category))
      LOG_ERROR(parameters_.location(PARAM_CATEGORIES) << ": category " << category << " is not a valid category");
  }
}

/**
 * Build any runtime objects an relationships for
 * this object
 */
void Ageing::Build() {
  partition_  = accessor::CategoriesPtr(new accessor::Categories(category_names_));
  model_      = Model::Instance();
}

/**
 *
 */
void Ageing::Execute() {
  for (auto iterator = partition_->Begin(); iterator != partition_->End(); ++iterator) {
    Double carry_over = 0.0;
    Double temp       = 0.0;

    for(Double& data : (*iterator)->data_) {
      temp = data;
      data = carry_over;
      carry_over = temp;
    }

    if (model_->age_plus()) {
      (* (*iterator)->data_.rbegin() ) += carry_over;
    }
  }
}

} /* namespace processes */
} /* namespace isam */
