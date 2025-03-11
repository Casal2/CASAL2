/**
 * @file Difference.h
 * @author A Dunn
 * @github https://github.com/Zaita
 * @date May, 2023
 * @section LICENSE
 *
 *
 * @section This methods takes two @estimate blocks everides the original methods with an estimate and a difference
 *
 * << Add Description >>
 */
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_DIFFERENCE_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_DIFFERENCE_H_

// headers
#include "../AddressableTransformation.h"

// namespaces
namespace niwa::addressabletransformations {

// classes
class Difference : public AddressableTransformation {
public:
  Difference() = delete;
  explicit Difference(shared_ptr<Model> model);
  virtual ~Difference() = default;
  Double GetScore() override final;
  void   FillReportCache(ostringstream& cache) override final;
  void   FillTabularReportCache(ostringstream& cache, bool first_run) override final;

  void PrepareForObjectiveFunction() override final;
  void RestoreForObjectiveFunction() override final;

protected:
  // methods
  void DoValidate() override final;
  void DoBuild() override final;
  void DoRestore() override final;

private:
  // members
  Double   first_parameter_;
  Double   second_parameter_;
  Double   difference_parameter_;
  unsigned number_of_parameters_;
};

} /* namespace niwa::addressabletransformations */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_DIFFERENCE_H_ */
