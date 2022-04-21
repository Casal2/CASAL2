/**
 * @file Logistic.h
 * @author C.Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_LOGISTIC_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_LOGISTIC_H_

// headers
#include "../AddressableTransformation.h"

// namespaces
namespace niwa {
namespace addressabletransformations {

/**
 *
 */
class Logistic : public AddressableTransformation {
public:
  Logistic() = delete;
  explicit Logistic(shared_ptr<Model> model);
  virtual ~Logistic() = default;
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
  Double    logistic_value_;
  Double    lower_bound_;
  Double    upper_bound_;

};

} /* namespace addressabletransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_LOGISTIC_H_ */
