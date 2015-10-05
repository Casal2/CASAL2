/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 17/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is the manager responsible for holding all of the estimates in the system
 * and providing access to the other systems like ObjectiveFunction, Reports etc.
 */
#ifndef ESTIMATE_MANAGER_H_
#define ESTIMATE_MANAGER_H_

// headers
#include <boost/enable_shared_from_this.hpp>

#include "BaseClasses/Manager.h"
#include "Estimates/Estimate.h"
#include "Estimates/Info.h"
#include "Estimates/Creators/Creator.h"
#include "Utilities/Map.h"

// namespaces
namespace niwa {
class Managers;

namespace estimates {

/**
 * Class definition
 */
class Manager : public niwa::oldbase::Manager<estimates::Manager, niwa::Estimate> {
  friend class niwa::oldbase::Manager<estimates::Manager, niwa::Estimate>;
  friend class niwa::Managers;
public:
  // methods
  virtual                     ~Manager() noexcept(true) { };
  void                        Validate() override final;
  unsigned                    GetEnabledCount();
  vector<Estimate*>           GetEnabled();
  void                        Clear() override final;
  bool                        HasEstimate(const string& parameter);
  void                        EnableEstimate(const string& parameter);
  void                        DisableEstimate(const string& parameter);
  void                        AddCreator(Creator* creator) { creators_.push_back(creator); }
  Estimate*                   GetEstimate(const string& parameter);
  Estimate*                   GetEstimateByLabel(const string& label);

private:
  // methods
  Manager() { };

  // members
  vector<Creator*>            creators_;
};

} /* namespace estimates */
} /* namespace niwa */
#endif /* ESTIMATE_MANAGER_H_ */
