/**
 * @file Categories.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 23/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifndef TESTRESOURCES_CONFIGURATION_AGE_SIZES_H_
#define TESTRESOURCES_CONFIGURATION_AGE_SIZES_H_

#include <string>

const std::string age_sizes_von_bert = R"(
@age_size von_bert
type von_bertalanffy
size_weight basic
k      0.091
t0    -0.117
linf   174.545
)";

const std::string age_sizes_von_bert_no_age_size = R"(
@age_size von_bert
type von_bertalanffy
size_weight none
k      0.2
t0     0.0
linf   100
)";

#endif /* MODEL_H_ */
