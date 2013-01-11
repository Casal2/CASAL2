/*
 * Factory.h
 *
 *  Created on: 21/12/2012
 *      Author: Admin
 */

#ifndef FACTORY_H_
#define FACTORY_H_

#include "BaseClasses/Factory.h"
#include "Selectivities/Selectivity.h"
#include "Selectivities/Manager.h"

namespace isam {
namespace selectivities {

class Factory : public isam::base::Factory<isam::Selectivity, isam::selectivities::Manager> {
public:
  static SelectivityPtr Create(const string& block_type, const string& process_type);
private:
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace selectivities */
} /* namespace isam */
#endif /* FACTORY_H_ */
