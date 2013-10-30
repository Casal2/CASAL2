/*
 * AgeSize.cpp
 *
 *  Created on: 24/07/2013
 *      Author: Admin
 */

#include "AgeSize.h"

namespace isam {

/**
 *
 */
AgeSize::AgeSize() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type");

}

/**
 *
 */
void AgeSize::Validate() {
  parameters_.Populate();

  DoValidate();
}

} /* namespace isam */
