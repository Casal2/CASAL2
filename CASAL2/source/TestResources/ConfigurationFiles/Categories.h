/**
 * @file Categories.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 23/08/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/�2013 - www.niwa.co.nz
 *
 */
#ifndef TESTRESOURCES_CONFIGURATION_CATEGORIES_H_
#define TESTRESOURCES_CONFIGURATION_CATEGORIES_H_

#include <string>

const std::string categories_no_sex_with_age_size = R"(
@categories
format stage.sex
names immature mature spawning
age_lengths von_bert von_bert von_bert
)";

const std::string categories_two_sex_with_age_size = R"(
@categories
format stage.sex
names immature.male immature.female mature.male mature.female
age_lengths von_bert von_bert von_bert von_bert
)";

#endif /* MODEL_H_ */
