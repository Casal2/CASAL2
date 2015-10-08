/**
 * @file Factory.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 8/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef FACTORY_H_
#define FACTORY_H_

// headers
#include <string>

// namespaces
namespace niwa {

using std::string;
class Model;
namespace base { class Object; }

/**
 * Class definition
 */
class Factory {
  friend class Model;
public:
  // methods
  base::Object*               CreateObject(string& object_type, string& sub_type);

private:
  // methods
  Factory(Model* model);
  virtual ~Factory() = default;

  // members
  Model*                      model_;
};

} /* namespace niwa */
#endif /* FACTORY_H_ */
