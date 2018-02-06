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

#include "Common/Utilities/To.h"
#include "Common/Categories/Categories.h"
#include "Common/TimeSteps/Manager.h"
#include "Common/Utilities/Map.h"

// Namespaces
namespace niwa {
namespace age {
namespace processes {

/**
 * Default constructor
 */
Ageing::Ageing(Model* model)
  : Process(model),
    partition_(model) {
  process_type_ = ProcessType::kAgeing;
  partition_structure_ = PartitionType::kAge;

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The labels of the categories", "");
}

/**
 * Validate our ageing process.
 *
 * 1. Check for any required parameters
 * 2. Assign the label from our parameters
 * 3. Assign any remaining parameters
 */
void Ageing::DoValidate() {
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
  partition_.Init(category_labels_);
}

/**
 * Execute our ageing class.
 */
void Ageing::DoExecute() {
  LOG_TRACE();
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

    if (model_->plus_group())
      (* category->data_.rbegin() ) += moved_fish;
  }
}

} /* namespace processes */
} /* namespace age */
} /* namespace niwa */
