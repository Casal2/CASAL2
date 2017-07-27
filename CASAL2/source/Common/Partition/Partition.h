/**
 * @file World.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class holds the partition for our model. It's responsible for ensuring
 * the World::Accessors can access the partition properly.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PARTITION_H_
#define PARTITION_H_

// Headers
#include <map>
#include <vector>
#include <string>
#include <memory>

#include "Common/Partition/Category.h"
#include "Common/Utilities/Types.h"

// Namespaces
namespace niwa {
class Model;

using std::string;
using std::map;
using std::vector;

/**
 * Class Definition
 */
class Partition {
  friend class Model;
public:
  // Methods
  virtual                     ~Partition();
  void                        Validate();
  void                        Build();
  void                        Reset();
  void                        Clear() { partition_.clear(); }

  // Accessors
  partition::Category&        category(const string& category_label);

private:
  // Methods
  Partition(Model* model) : model_(model) { };

  // Members
  Model*                      model_ = nullptr;
  map<string, partition::Category*> partition_; // map<category label, partition::Category Struct>
};

} /* namespace niwa */
#endif /* PARTITION_H_ */
