/**
 * @file ObjectiveFunction.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 1/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "ObjectiveFunction.h"

#include "Model/Model.h"

// namespaces
namespace isam {
namespace asserts {

/**
 *
 */
ObjectiveFunction::ObjectiveFunction() {
}

/**
 *
 */
void ObjectiveFunction::DoValidate() { }

/**
 *
 */
void ObjectiveFunction::DoBuild() {
  Model::Instance()->Subscribe(State::kFinalise, shared_ptr());
}

/**
 *
 */
void ObjectiveFunction::Execute() {

}

} /* namespace asserts */
} /* namespace isam */
