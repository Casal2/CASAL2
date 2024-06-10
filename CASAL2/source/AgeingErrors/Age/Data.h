/**
 * @file Data.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 19/06/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * Data AgeingErrors is a user defined table that describes the probability of a fish being aged as age i being aged as age j.
 *
 */
#ifndef AGEINGERRORS_DATA_H_
#define AGEINGERRORS_DATA_H_

// headers
#include "../../AgeingErrors/AgeingError.h"

// namespaces
namespace niwa {
namespace ageingerrors {

// classes
class Data : public niwa::AgeingError {
public:
  // methods
  Data(shared_ptr<Model> model);
  virtual ~Data();

protected:
  // methods
  void DoValidate() override final{};
  void DoBuild() override final;
  void DoReset() override final{};

private:
  // members
  parameters::Table* data_table_ = nullptr;
  Double             tol_        = 1e-5;
};

} /* namespace ageingerrors */
} /* namespace niwa */
#endif /* AGEINGERRORS_DATA_H_ */
