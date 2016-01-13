/**
 * @file dll.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 12, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file is the entry point to our DLL shared library that is loaded by the front
 * end.
 */
#ifndef SOURCE_DLL_H_
#define SOURCE_DLL_H_

#include "Utilities/RunParameters.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EXAMPLE_DLL __declspec(dllexport)
// NOTE: this function is not declared extern "C"

int EXAMPLE_DLL RunUnitTests(int argc, char * argv[]);
void EXAMPLE_DLL LoadOptions(int argc, char * argv[], niwa::utilities::RunParameters& options);
int EXAMPLE_DLL Run(int argc, char * argv[], niwa::utilities::RunParameters& options);

#ifdef __cplusplus
}
#endif

#endif /* SOURCE_DLL_H_ */
