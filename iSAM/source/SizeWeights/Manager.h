/**
 * @file Manager.h
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
#ifndef SIZEWEIGHTS_MANAGER_H_
#define SIZEWEIGHTS_MANAGER_H_

// headers
#include "BaseClasses/Manager.h"
#include "SizeWeights/SizeWeight.h"

// namespaces
namespace niwa {
namespace sizeweights {

/**
 * class definition
 */
class Manager : public niwa::oldbase::Manager<niwa::sizeweights::Manager, niwa::SizeWeight> {
  friend class niwa::oldbase::Manager<niwa::sizeweights::Manager, niwa::SizeWeight>;
public:
  // methods
  virtual                     ~Manager() noexcept(true) { };
  SizeWeightPtr               GetSizeWeight(const string& label) const;

protected:
  // methods
  Manager();
};

} /* namespace sizeweights */
} /* namespace niwa */
#endif /* SIZEWEIGHTS_MANAGER_H_ */
