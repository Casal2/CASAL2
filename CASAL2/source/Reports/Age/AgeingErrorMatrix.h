/*
 * AgeingErrorMatrix.h
 *
 *  Created on: 10.9.2015
 *      Author: Admin
 */

#ifndef AGE_REPORTS_AGEINGERRORMATRIX_H_
#define AGE_REPORTS_AGEINGERRORMATRIX_H_

// headers
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
class AgeingError;
namespace reports {
namespace age {
/**
 *
 */
class AgeingErrorMatrix : public niwa::Report {
public:
  AgeingErrorMatrix();
  virtual                     ~AgeingErrorMatrix() = default;
  void                        DoValidate(shared_ptr<Model> model) final { };
  void                        DoBuild(shared_ptr<Model> model) final;
  void                        DoExecute(shared_ptr<Model> model) final;
  void                        DoExecuteTabular(shared_ptr<Model> model) final { };

private:
  string                      ageing_error_label_ = "";
};

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */

#endif /* AGE_REPORTS_AGEINGERRORMATRIX_H_ */
