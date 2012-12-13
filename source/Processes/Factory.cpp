/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Factory.h"

#include "Processes/Children/Ageing.h"
#include "Processes/Children/ConstantMortality.h"
#include "Processes/Children/ConstantRecruitment.h"

// Namespaces
namespace isam {
namespace processes {

/**
 *
 */
ProcessPtr Factory::Create(const string& block_type, const string& process_type) {

  ProcessPtr result;

  if (block_type == "recruitment" && process_type == "constant") {
    result = ProcessPtr(new ConstantRecruitment());

  }

  //if (result)
  //  ProcessManager::Instance().Add(result);

  return result;
}

} /* namespace processes */
} /* namespace isam */
