/**
 * @file Inverse.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 8, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_INVERSE_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_INVERSE_H_

// headers
#include "../AddressableTransformation.h"

// namespaces
namespace niwa {
namespace addressabletransformations {

// classes
class Inverse : public AddressableTransformation {
public:
  Inverse() = delete;
  explicit Inverse(shared_ptr<Model> model);
  virtual ~Inverse() = default;
  Double           GetScore() override final;
  void             FillReportCache(ostringstream& cache) override final;
  void             FillTabularReportCache(ostringstream& cache, bool first_run) override final;

  void             PrepareForObjectiveFunction() override final;
  void             RestoreForObjectiveFunction() override final;

protected:
  // methods
  void DoValidate() override final;
  void DoBuild() override final;
  void DoRestore() override final;

  Double          inverse_value_;

};

} /* namespace addressabletransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_INVERSE_H_ */
