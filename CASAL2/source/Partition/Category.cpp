/**
 * @file Category.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 18/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 */

// headers
#include "Category.h"

#include <functional>

#include "../AgeLengths/AgeLength.h"
#include "../Categories/Categories.h"
#include "../LengthWeights/LengthWeight.h"
#include "../Model/Model.h"
#include "../Partition/Partition.h"
#include "../TimeSteps/Manager.h"
#include "../Utilities/Math.h"
#include "../Utilities/To.h"
#include "../Utilities/Types.h"

// namespaces
namespace niwa {
namespace partition {

/**
 * @brief 
 *
 */
void Category::Cache() {
  if (cached_data_.size() != data_.size())
    cached_data_ = data_;
  else {
    for (unsigned i = 0; i < data_.size(); ++i) {
      cached_data_[i] = data_[i];
    }
  }
}

}  // namespace partition
} /* namespace niwa */
