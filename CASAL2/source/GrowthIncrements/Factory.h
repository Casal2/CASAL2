/**
 * @file Factory.h
 * @author  C.Marsh
 * @date 2022
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef GROWTH_INCREMENT_FACTORY_H_
#define GROWTH_INCREMENT_FACTORY_H_

// headers
#include "../GrowthIncrements/GrowthIncrement.h"

// namespaces
namespace niwa {
class Model;

namespace growthincrements {

/**
 * class definition
 */
class Factory {
public:
  // methods
  static GrowthIncrement* Create(shared_ptr<Model> model, const string& object_type, const string& sub_type);

private:
  // methods
  Factory()          = delete;
  virtual ~Factory() = delete;
};

} /* namespace growthincrements */
} /* namespace niwa */
#endif /* GROWTH_INCREMENT_FACTORY_H_ */
