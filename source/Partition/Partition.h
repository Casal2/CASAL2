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

#include "Utilities/Types.h"

// Namespaces
namespace isam {

using std::string;
using std::map;
using std::vector;
using isam::utilities::Double;

enum ModelType {
  kLengthModel,
  kAgeModel,
  kCustomModel
};

/**
 * Class Definition
 */
class Partition {
public:
  // Methods
  virtual                     ~Partition() = default;
  static Partition&           Instance();
  void                        Validate();
  void                        Build();

  // Accessors
  map<string, vector<Double> >& grid() { return grid_; }

private:
  // Methods
  Partition() = default;

  // Members
  map<string, vector<Double> >  grid_;  // 2D Array map<Category, vector<Age/Length/Bin> >
};

} /* namespace isam */
#endif /* WORLD_H_ */
