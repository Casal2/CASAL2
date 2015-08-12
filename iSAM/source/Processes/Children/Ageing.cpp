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
#include "TimeSteps/Manager.h"
#include "Utilities/Map.h"

// Namespaces
namespace niwa {
namespace processes {

/**
 * Default constructor
 */
Ageing::Ageing() {
  process_type_ = ProcessType::kAgeing;
  partition_structure_ = PartitionStructure::kAge;

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_names_, "Categories", "");
}

/**
 * Validate our ageing process.
 *
 * 1. Check for any required parameters
 * 2. Assign the label from our parameters
 * 3. Assign any remaining parameters
 */
void Ageing::DoValidate() {
  // Ensure defined categories were valid
  for(const string& category : category_names_) {
    if (!Categories::Instance()->IsValid(category))
      LOG_ERROR_P(PARAM_CATEGORIES) << ": category " << category << " is not a valid category";
  }
}

/**
 * Build objects that are needed by this object during the execution phase. This
 * includes things like the partition accessor it will need.
 *
 * Then build values that we want to print when print is called.
 *
 * Then build the basics
 */
void Ageing::DoBuild() {
  partition_.Init(category_names_);
  model_      = Model::Instance();
}

/**
 * Execute our ageing class.
 */
void Ageing::DoExecute() {
  Double amount_to_move = 0.0;
  Double moved_fish = 0.0;

  for (auto category : partition_) {
    moved_fish = 0.0;
    for (Double& data : category->data_) {
      amount_to_move = data;
      data -= amount_to_move;
      data += moved_fish;

      moved_fish = amount_to_move;
    }

    if (model_->age_plus())
      (* category->data_.rbegin() ) += moved_fish;
  }
}

} /* namespace processes */
} /* namespace niwa */
