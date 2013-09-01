/**
 * @file EmptyModel.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 9/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include "EmptyModel.h"

#include "Model/Model.h"

// namespaces
namespace isam {
namespace testfixtures {

/**
 *
 */
void EmptyModel::SetUp() {
  Model::Instance(true);
  Model::Instance()->set_run_mode(RunMode::kTesting);
}


} /* namespace testfixtures */
} /* namespace isam */
#endif
