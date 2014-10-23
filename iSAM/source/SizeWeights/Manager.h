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
namespace isam {
namespace sizeweights {

/**
 * class definition
 */
class Manager : public isam::oldbase::Manager<isam::sizeweights::Manager, isam::SizeWeight> {
  friend class isam::oldbase::Manager<isam::sizeweights::Manager, isam::SizeWeight>;
public:
  // methods
  virtual                     ~Manager() noexcept(true) { };
  SizeWeightPtr               GetSizeWeight(const string& label) const;

protected:
  // methods
  Manager();
};

} /* namespace sizeweights */
} /* namespace isam */
#endif /* SIZEWEIGHTS_MANAGER_H_ */
