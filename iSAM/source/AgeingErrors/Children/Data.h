/**
 * @file Data.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 19/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * TODO: Add documentation
 * TODO: Implement this class
 */
#ifndef AGEINGERRORS_DATA_H_
#define AGEINGERRORS_DATA_H_

// headers
#include "AgeingErrors/AgeingError.h"

// namespaces
namespace niwa {
namespace ageingerrors {

// classes
class Data : public niwa::AgeingError {
public:
  // methods
  Data();
  virtual                   ~Data() = default;

protected:
  // methods
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;

private:
  // members
  parameters::TablePtr          data_table_;
};

} /* namespace ageingerrors */
} /* namespace niwa */
#endif /* AGEINGERRORS_DATA_H_ */
