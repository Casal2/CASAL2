/**
 * @file SquareRoot.h
 * @author A Dunn
 * @date Nov, 2022
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SQUAREROOT_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SQUAREROOT_H_

// headers
#include "../AddressableTransformation.h"

// namespaces
namespace niwa {
namespace addressabletransformations {

/**
 *
 */
class SquareRoot : public AddressableTransformation {
public:
  SquareRoot() = delete;
  explicit SquareRoot(shared_ptr<Model> model);
  virtual ~SquareRoot() = default;
  Double GetScore() override final;
  void   FillReportCache(ostringstream& cache) override final;
  void   FillTabularReportCache(ostringstream& cache, bool first_run) override final;
  void   PrepareForObjectiveFunction() override final;
  void   RestoreForObjectiveFunction() override final;

protected:
  // methods
  void   DoValidate() override final;
  void   DoBuild() override final;
  void   DoRestore() override final;
  Double SquareRoot_value_;
};

} /* namespace addressabletransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SQUAREROOT_H_ */
