/**
 * @file Ageing.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This is a simple ageing process. It's responsible for ageing all
 * fish by 1 each time it's called.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef AGEING_H_
#define AGEING_H_

// Headers
#include "Partition/Accessors/Categories.h"
#include "Processes/Process.h"

// Namespaces
namespace niwa {
namespace processes {
namespace age {

namespace accessor = niwa::partition::accessors;

/**
 * Class Definition
 */
class Ageing : public niwa::Process {
public:
  // Methods
  explicit Ageing(shared_ptr<Model> model);
  virtual ~Ageing() = default;
  void                  DoValidate() override final;
  void                  Verify(shared_ptr<Model> model){};
  void                  DoBuild() override final;
  void                  DoReset() override final{};
  void                  DoExecute() override final;
  const vector<string>& category_labels() const { return category_labels_; }

private:
  // Members
  accessor::Categories partition_;
  vector<string>       category_labels_;
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */

#endif /* AGEING_H_ */
