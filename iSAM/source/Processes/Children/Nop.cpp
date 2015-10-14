/**
 * @file Nop.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file exists to keep documentation generator happy
 */

// headers
#include "Nop.h"

// namespaces
namespace niwa {
namespace processes {

/**
 * Empty constructor
 */
Nop::Nop(Model* model) : Process(model) {
  process_type_ = ProcessType::kAgeing;
  partition_structure_ = PartitionStructure::kAny;
}

}
}
