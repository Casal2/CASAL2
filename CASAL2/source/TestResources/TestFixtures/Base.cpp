/**
 * @file Base.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2/09/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include "Base.h"

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"

// namespaces
namespace niwa {
namespace testfixtures {

/**
 *
 */
Base::Base() { }

/**
 *
 */
Base::~Base() { }

/**
 *
 */
void Base::SetUp() {
  model_ = new Model();
}

/**
 *
 */
void Base::TearDown() {
  delete model_;
}

} /* namespace testfixtures */
} /* namespace niwa */
#endif /* TESTMODE */
