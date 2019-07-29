/**
 * @file fPartitionType.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file contains the list of model partition types
 */

#ifndef UTILITIES_PARTITIONTYPE_H_
#define UTILITIES_PARTITIONTYPE_H_

#include <string>

// Enumerated Types
enum class PartitionType {
  kAge      = 1,
  kLength   = 2,
//  kHybrid   = 4,
  kModel    = 2048,
  kInvalid  = 4096
};

inline PartitionType operator&(PartitionType a, PartitionType b) {
  return static_cast<PartitionType>(static_cast<int>(a) & static_cast<int>(b));
}

inline PartitionType operator|(PartitionType a, PartitionType b) {
  return static_cast<PartitionType>(static_cast<int>(a) | static_cast<int>(b));
}

#endif /* UTILITIES_PARTITIONTYPE_H_ */
