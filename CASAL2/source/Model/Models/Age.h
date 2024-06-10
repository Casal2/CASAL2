/**
 * @file Age.h
 * @author  Scott Rasmussen (scott@zaita.com)
 * @date 20/09/2019
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @description
 * This is a specialisation of our core model object that is for age based models.
 */
#ifndef SOURCE_MODEL_MODELS_AGE_H_
#define SOURCE_MODEL_MODELS_AGE_H_

// headers
#include "../../Model/Model.h"

// namespaces
namespace niwa::model {

// class declaration
class Age : public Model {
public:
  // methods
  Age();
  virtual ~Age() = default;
  void DoValidate() override;
};

}  // namespace niwa::model

#endif /* SOURCE_MODEL_MODELS_AGE_H_ */
