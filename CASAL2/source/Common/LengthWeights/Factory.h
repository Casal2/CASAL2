/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef LENGTHWEIGHTS_FACTORY_H_
#define LENGTHWEIGHTS_FACTORY_H_

// headers
#include "Common/LengthWeights/LengthWeight.h"

// namespaces
namespace niwa {
class Model;

namespace lengthweights {

/**
 * class definition
 */
class Factory {
public:
  // methods
  static LengthWeight*        Create(Model* model, const string& object_type, const string& sub_type);

private:
  // methods
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace lengthweights */
} /* namespace niwa */
#endif /* LENGTHWEIGHTS_FACTORY_H_ */
