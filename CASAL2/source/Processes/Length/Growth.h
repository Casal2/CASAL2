/**
 * @file Growth.h
 * @author  C.Marsh
 * @version 1.0
 * @date 27/07/2017
 * @section LICENSE
 *
 * Copyright NIWA Science �2017 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Growth is the class responsible for moving numbers at length to other length bins within a category, Can be thought of as analagous to ageing in an age based model
 *
 *
 */
#ifndef LENGTH_GROWTH_H_
#define LENGTH_GROWTH_H_

// Headers
#include "../../Partition/Accessors/Categories.h"
#include "../../Processes/Process.h"

// Namespaces
namespace niwa {
namespace processes {
namespace length {

namespace accessor = niwa::partition::accessors;

/**
 * Class Definition
 */
class Growth : public niwa::Process {
public:
  // Methods
  explicit Growth(shared_ptr<Model> model);
  virtual ~Growth() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoExecute() override final;
  const vector<string>& category_labels() const { return category_labels_; }

private:
  // Members
  accessor::Categories   partition_;
  vector<string>         category_labels_;
  vector<Double>         new_length_partition_;
};

} /* namespace length */
} /* namespace processes */
} /* namespace niwa */
#endif /* LENGTH_GROWTH_BASIC_H_ */
