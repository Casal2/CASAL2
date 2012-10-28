/**
 * @file World.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class holds the partition for our model. It's responsible for ensuring
 * the World::Accessors can access the partition properly.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef WORLD_H_
#define WORLD_H_

// Headers
#include <map>
#include <vector>
#include <string>
#include "../Utilities/Types.h"

// Namespaces
namespace iSAM {

using std::string;
using std::map;
using std::vector;
using iSAM::Utilities::Double;

/**
 * Class Definition
 */
class Partition {
public:
  // Methods
  Partition();
  virtual ~Partition();

private:
  // Members
  vector<vector<Double> >     partition_; // 2D Array (Category[H] x Age[W])
  vector<vector<Double> >     adjustment_partition_; // Partition that holds modifications to be made
};

} /* namespace iSAM */
#endif /* WORLD_H_ */
