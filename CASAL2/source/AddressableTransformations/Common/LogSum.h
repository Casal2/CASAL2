/**
 * @file LogSum.h
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date Jul 8, 2017
 * @section LICENSE
 *
 * Copyright NIWA Science �2017 - www.niwa.co.nz
 *
 * @section
 * This transformation takes two parameters (x1 and x2) and estimates y1 = log(x1 + x2) and p1 where x1 = exp(y1)*p1 and x2 = exp(y1)* (1 - p1)
 * Used in the hoki stock assessment to when they have two stock B0 estimates for some reason they choice to estimate prop and log B0
 */
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_LOGSUM_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_LOGSUM_H_

// headers
#include "../EstimableTransformation.h"

// namespaces
namespace niwa {
namespace addressableransformations {

// classes
class LogSum : public EstimableTransformation {
public:
  LogSum() = delete;
  explicit LogSum(shared_ptr<Model> model);
  virtual ~LogSum() = default;
  Double           GetScore() override final;
  void             FillReportCache(ostringstream& cache) override final;
  Double           GetRestoredValue(unsigned index) override final;

protected:
  // methods
  void DoValidate() override final;
  void DoBuild() override final;
  void DoRestore() override final;

private:
  // members
  Double          log_total_parameter_;
  Double          total_difference_parameter_;
  unsigned        number_of_parameters_;
  Double          total_parameter_;

};

} /* namespace estimabletransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_LOGSUM_H_ */
