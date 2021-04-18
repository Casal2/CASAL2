/**
 * @file dll.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 12, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science (c)2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file is the entry point to our DLL shared library that is loaded by the front
 * end.
 */
#ifndef SOURCE_SHAREDLIBRARY_H_
#define SOURCE_SHAREDLIBRARY_H_
#include "Utilities/RunParameters.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __MINGW32__
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

int EXPORT RunUnitTests(int argc, char* argv[]);
int EXPORT LoadOptions(int argc, char* argv[], niwa::utilities::RunParameters& options);
int EXPORT PreParseConfigFiles(niwa::utilities::RunParameters& options);
int EXPORT Run(int argc, char* argv[], niwa::utilities::RunParameters& options);

#ifdef __cplusplus
}
#endif /* _cplusplus */
#endif /* SOURCE_SHAREDLIBRARY_H_ */
