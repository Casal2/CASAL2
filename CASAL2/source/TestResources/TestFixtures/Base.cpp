/**
 * @file Base.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2/09/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

// headers
#include "Base.h"

#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../Model/Models/Age.h"
#include "../../Model/Models/Length.h"

// namespaces
namespace niwa {
namespace testfixtures {

/**
 *
 */
void Base::SetUp() {
  model_.reset(new model::Age());
  model_->set_global_configuration(&global_config_);
  model_->managers()->create_test_managers();
}

/**
 *
 */
void Base::TearDown() {}

} /* namespace testfixtures */
} /* namespace niwa */
#endif /* TESTMODE */
