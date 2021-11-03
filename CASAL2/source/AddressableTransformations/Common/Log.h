/**
 * @file Log.h
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
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_LOG_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_LOG_H_

// headers
#include "../EstimableTransformation.h"

// namespaces
namespace niwa {
namespace addressableransformations {

/**
 *
 */
class Log : public EstimableTransformation {
public:
  Log() = delete;
  explicit Log(shared_ptr<Model> model);
  virtual ~Log() = default;
  Double           GetScore() override final;
  void             FillReportCache(ostringstream& cache) override final;
  Double           GetRestoredValue(unsigned index) override final;

protected:
  // methods
  void DoValidate() override final;
  void DoBuild() override final;
  void DoRestore() override final;
  Double    log_value_;

};

} /* namespace estimabletransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_LOG_H_ */
