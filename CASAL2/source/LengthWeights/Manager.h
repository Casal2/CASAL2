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
#ifndef LENGTHWEIGHTS_MANAGER_H_
#define LENGTHWEIGHTS_MANAGER_H_

// headers
#include "../BaseClasses/Manager.h"
#include "../LengthWeights/LengthWeight.h"

// namespaces
namespace niwa {
namespace lengthweights {

/**
 * class definition
 */
class Manager : public niwa::base::Manager<niwa::lengthweights::Manager, niwa::LengthWeight> {
  friend class niwa::base::Manager<niwa::lengthweights::Manager, niwa::LengthWeight>;
  friend class niwa::Managers;

public:
  // methods
  virtual ~Manager() noexcept(true){};
  LengthWeight* GetLengthWeight(const string& label) const;

protected:
  // methods
  Manager();
};

} /* namespace lengthweights */
} /* namespace niwa */
#endif /* LENGTHWEIGHTS_MANAGER_H_ */
