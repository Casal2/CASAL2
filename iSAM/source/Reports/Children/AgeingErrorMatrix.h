/*
 * AgeingErrorMatrix.h
 *
 *  Created on: 10.9.2015
 *      Author: Admin
 */

#ifndef REPORTS_AGEINGERRORMATRIX_H_
#define REPORTS_AGEINGERRORMATRIX_H_

// headers
#include "Reports/Report.h"
#include "AgeingErrors/Manager.h"
// namespaces
namespace niwa {
namespace reports {

/**
 *
 */
class AgeingErrorMatrix : public niwa::Report {
public:
  AgeingErrorMatrix();
  virtual                     ~AgeingErrorMatrix() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final;
  void                        DoExecute() override final;

private:
  string                      ageingerror_label_;
  AgeingErrorPtr              ageingerror_;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_AGEINGERRORMATRIX_H_ */
