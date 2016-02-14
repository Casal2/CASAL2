/*
 * RunMode.h
 *
 *  Created on: 25/10/2013
 *      Author: Admin
 */

#ifndef UTILITIES_RUNMODE_H_
#define UTILITIES_RUNMODE_H_

// Enumerated Types
namespace RunMode {
enum Type {
  kInvalid      = 1,
  kLicense      = 2,
  kVersion      = 3,
  kHelp         = 4,
  kBasic        = 8,
  kEstimation   = 16,
  kMCMC         = 32,
  kSimulation   = 64,
  kProfiling    = 128,
  kProjection   = 256,
  kQuery        = 512,
  kTesting      = 4096,
  kUnitTest     = 8192
};
}

#endif /* RUNMODE_H_ */
