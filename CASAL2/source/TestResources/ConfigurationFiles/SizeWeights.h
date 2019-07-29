/**
 * @file SizeWeights.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 23/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifndef TESTRESOURCES_CONFIGURATION_SIZE_WEIGHTS_H_
#define TESTRESOURCES_CONFIGURATION_SIZE_WEIGHTS_H_

#include <string>

const std::string size_weights_basic = R"(
@length_weight basic
type basic
a 1.051e-008
b 3.036
units tonnes
)";

const std::string size_weights_none = R"(
@length_weight none
type none
)";

#endif /* MODEL_H_ */
