/**
 * @file Partition.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This class holds the partition for the model. It is responsible for ensuring
 * the World::Accessors can access the partition properly.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PARTITION_H_
#define PARTITION_H_

// Headers
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../Partition/Category.h"
#include "../Utilities/NoCopy.h"
#include "../Utilities/Types.h"

// Namespaces
namespace niwa {
class Model;

using std::map;
using std::string;
using std::vector;

/**
 * Class Definition
 */
class Partition {
  friend class Model;

public:
  // Methods
  virtual ~Partition();
  void Validate();
  void Build();
  void Verify(shared_ptr<Model> model){};
  void Reset();
  void Clear() { partition_.clear(); }

  // Accessors
  partition::Category& category(const string& category_label);

protected:
  // Methods
  Partition(shared_ptr<Model> model) : model_(model){};

  // Members
  shared_ptr<Model>                 model_ = nullptr;
  map<string, partition::Category*> partition_;  // map<category label, partition::Category Struct>

  DISALLOW_COPY_AND_ASSIGN(Partition);
};

} /* namespace niwa */
#endif /* PARTITION_H_ */
