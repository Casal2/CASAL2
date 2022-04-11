/**
 * @file SumToOne.h
 * @author C.Marsh
 * @github https://github.com/
 * @date Jan/11/2018
 * @section LICENSE
 *
 * Copyright NIWA Science �2018 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Allows users to estimate a series K parameters that must sum to one as K-1 parameters by allowing the K parameter = 1 - sum of k-1 parameters
 */
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SUM_TO_ONE_
#define SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SUM_TO_ONE_

// headers
#include "../AddressableTransformation.h"

// namespaces
namespace niwa {
namespace addressabletransformations {

/**
 *
 */

class SumToOne : public AddressableTransformation {
public:
  SumToOne() = delete;
  explicit SumToOne(shared_ptr<Model> model);
  virtual ~SumToOne() = default;
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
  Double difference_parameter_;
};

} /* namespace addressabletransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SUM_TO_ONE_ */
