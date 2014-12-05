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

#include "AgeSizes/AgeSize.h"
#include "Utilities/Types.h"

// Namespaces
namespace niwa {

using std::string;
using std::map;
using std::vector;
using niwa::utilities::Double;

enum ModelType {
  kLengthModel,
  kAgeModel,
  kCustomModel
};

namespace partition {
struct Category {
  string    name_;
  unsigned  min_age_;
  unsigned  max_age_;
  vector<unsigned>  years_;
  vector<Double>  data_;
  AgeSizePtr age_size_weight_ = AgeSizePtr();

//  map<unsigned, vector<Double>>  mean_weights_by_year_;
};
} /* namespace partition */

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
  void                        Reset();
//  void                        CalculateMeanWeights();
  void                        Clear() { partition_.clear(); }
  void                        Debug();


  // Accessors
  partition::Category&        category(const string& category_label);

private:
  // Methods
  Partition() = default;

  // Members
  map<string, partition::Category> partition_; // map<Category Name, partition::Category Struct>
};

} /* namespace niwa */
#endif /* WORLD_H_ */
