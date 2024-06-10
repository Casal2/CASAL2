/**
 * @file Length.h
 * @author  Scott Rasmussen (scott@zaita.com)
 * @date 20/09/2019
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 */
#ifndef SOURCE_MODEL_MODELS_LENGTH_H_
#define SOURCE_MODEL_MODELS_LENGTH_H_

// headers
#include "../../Model/Model.h"

// namespaces
namespace niwa {
namespace model {

// class declaration
class Length : public Model {
public:
  Length();
  virtual ~Length();
  void DoValidate() override;

};

} /* namespace model */
} /* namespace niwa */

#endif /* SOURCE_MODEL_MODELS_LENGTH_H_ */
