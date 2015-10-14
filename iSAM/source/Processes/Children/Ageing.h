/**
 * @file Ageing.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
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

namespace accessor = niwa::partition::accessors;

/**
 * Class Definition
 */
class Ageing : public niwa::Process {
public:
  // Methods
  explicit Ageing(Model* model);
  virtual                     ~Ageing() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        DoExecute() override final;

private:
  // Members
  accessor::Categories        partition_;
  vector<string>              category_names_;
};

} /* namespace processes */
} /* namespace niwa */
#endif /* AGEING_H_ */
