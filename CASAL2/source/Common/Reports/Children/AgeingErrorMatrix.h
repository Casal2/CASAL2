/*
 * AgeingErrorMatrix.h
 *
 *  Created on: 10.9.2015
 *      Author: Admin
 */

#ifndef REPORTS_AGEINGERRORMATRIX_H_
#define REPORTS_AGEINGERRORMATRIX_H_

// headers
#include "Common/Reports/Report.h"

// namespaces
namespace niwa {
class AgeingError;

namespace reports {

/**
 *
 */
class AgeingErrorMatrix : public niwa::Report {
public:
  AgeingErrorMatrix(Model* model);
  virtual                     ~AgeingErrorMatrix() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final;
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final { };

private:
  string                      ageingerror_label_ = "";
  AgeingError*                ageingerror_ = nullptr;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_AGEINGERRORMATRIX_H_ */
