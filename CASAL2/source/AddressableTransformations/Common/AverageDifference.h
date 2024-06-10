/**
 * @file AverageDifference.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 8, 2016
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section This methods takes two @estimate blocks everides the original methods with an estimate mean and difference
 *
 * << Add Description >>
 */
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_AVERAGEDIFFERENCE_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_AVERAGEDIFFERENCE_H_

// headers
#include "../AddressableTransformation.h"

// namespaces
namespace niwa {
namespace addressabletransformations {

// classes
class AverageDifference : public AddressableTransformation {
public:
  AverageDifference() = delete;
  explicit AverageDifference(shared_ptr<Model> model);
  virtual ~AverageDifference() = default;
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
private:
  // members
  Double          average_parameter_;
  Double          difference_parameter_;
  unsigned        number_of_parameters_;

};

} /* namespace addressabletransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_AVERAGEDIFFERENCE_H_ */
