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
@size_weight basic
type basic
a 1.051e-008
b 3.036
)";

const std::string size_weights_none = R"(
@size_weight none
type none
)";

#endif /* MODEL_H_ */
