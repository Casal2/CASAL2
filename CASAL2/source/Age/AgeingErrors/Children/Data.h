/**
 * @file Data.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 19/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Data AgeingErrors is a user defined table that describes the probability of a fish being aged as age i being aged as age j.
 *
 */
#ifndef AGEINGERRORS_DATA_H_
#define AGEINGERRORS_DATA_H_

// headers
#include "Age/AgeingErrors/AgeingError.h"

// namespaces
namespace niwa {
namespace ageingerrors {

// classes
class Data : public niwa::AgeingError {
public:
  // methods
  Data(Model* model);
  virtual                   ~Data();

protected:
  // methods
  void                        DoValidate() override final { };
  void                        DoBuild() override final;
  void                        DoReset() override final { };

private:
  // members
  parameters::Table*          data_table_ = nullptr;
};

} /* namespace ageingerrors */
} /* namespace niwa */
#endif /* AGEINGERRORS_DATA_H_ */
