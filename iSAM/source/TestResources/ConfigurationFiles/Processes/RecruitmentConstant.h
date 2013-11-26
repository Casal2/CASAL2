/**
 * @file RecruitmentConstant.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 23/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifndef TESTRESOURCES_CONFIGURATION_RECRUITMENT_CONSTANT_H_
#define TESTRESOURCES_CONFIGURATION_RECRUITMENT_CONSTANT_H_

#include <string>

const std::string recruitment_constant_no_sex = R"(
@recruitment recruitment_constant
type constant
categories immature
proportions 0.5
R0 997386
age 1
)";


const std::string recruitment_constant_two_sex = R"(
@recruitment recruitment_constant
type constant
categories immature.male immature.female
proportions 0.5 0.5
R0 997386
age 1
)";

const string constant_recruitment_no_sex_100 = R"(
@recruitment recruitment_constant
type constant
categories immature
proportions 1.0
r0 100
age 1
)";

#endif /* MODEL_H_ */
