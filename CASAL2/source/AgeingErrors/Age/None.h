/**
 * @file None.h
 * @author C. Marsh
 * @github https://github.com/Zaita
 * @date 11/08/2016
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * Data AgeingErrors allows you to have
 *
 */
#ifndef AGEINGERRORS_NONE_H_
#define AGEINGERRORS_NONE_H_

// headers
#include "../../AgeingErrors/AgeingError.h"

// namespaces
namespace niwa {
namespace ageingerrors {

// classes
class None : public niwa::AgeingError {
public:
  // methods
  None(shared_ptr<Model> model);
  virtual ~None();

protected:
  // methods
  void DoValidate() override final{};
  void DoBuild() override final;
  void DoReset() override final{};
};

} /* namespace ageingerrors */
} /* namespace niwa */
#endif /* AGEINGERRORS_NONE_H_ */
