/**
 * @file BaseLength.cpp
 * @author  C.Marsh
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include "BaseLength.h"

#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../Model/Models/Length.h"

// namespaces
namespace niwa {
namespace testfixtures {

/**
 *
 */
void BaseLength::SetUp() {
  model_.reset(new model::Length());
  model_->set_global_configuration(&global_config_);
  model_->managers()->create_test_managers();
}

/**
 *
 */
void BaseLength::TearDown() {}

} /* namespace testfixtures */
} /* namespace niwa */
#endif /* TESTMODE */
