/**
 * @file Data.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 22/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGESIZES_DATA_H_
#define AGESIZES_DATA_H_

// headers
#include "AgeSizes/AgeSize.h"

// namespaces
namespace isam {
namespace agesizes {

/**
 * Class definition
 */
class Data : public AgeSize {
public:
  Data();
  virtual                     ~Data() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final {};
};

} /* namespace agesizes */
} /* namespace isam */

#endif /* AGESIZES_DATA_H_ */
