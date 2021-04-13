/*
 * Partition.h
 *
 *  Created on: 10/10/2018
 *      Author: Zaita
 */

#ifndef SOURCE_ASSERTS_COMMON_PARTITION_H_
#define SOURCE_ASSERTS_COMMON_PARTITION_H_

// headers
#include "../../Asserts/Assert.h"

// namespaces
namespace niwa {
class Partition;
namespace asserts {

// class
class Partition : public niwa::Assert {
public:
  // methods
  Partition(shared_ptr<Model> model);
  virtual                     ~Partition() = default;
  void                        Execute() override final;

protected:
  // methods
  void                        DoValidate() override final;
  void                        DoBuild() override final;

private:
  // members
  string                      category_label_ = "";
  vector<Double>              values_;
}; // class

} /* namespace asserts */
} /* namespace niwa */

#endif /* SOURCE_ASSERTS_COMMON_PARTITION_H_ */
