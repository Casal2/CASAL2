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
namespace estimates {

using utilities::Map;

/**
 * Class definition
 */
class Manager : public niwa::oldbase::Manager<estimates::Manager, niwa::Estimate>, public boost::enable_shared_from_this<Manager>  {
  friend class niwa::oldbase::Manager<estimates::Manager, niwa::Estimate>;
public:
  // methods
  virtual                         ~Manager() noexcept(true) { };
  void                            Validate() override final;
  unsigned                        GetEnabledCount();
  vector<EstimatePtr>             GetEnabled();
  void                            RemoveAllObjects() override final;
  bool                            HasEstimate(const string& parameter);
  void                            EnableEstimate(const string& parameter);
  void                            DisableEstimate(const string& parameter);
  void                            AddCreator(CreatorPtr creator) { creators_.push_back(creator); }
  EstimatePtr                     GetEstimate(const string& parameter);
  void                            AddEstimateValue(const string& estimate_label, Double value);

  // accessors
  unsigned                        estimate_value_count() { if (estimate_values_.size() == 0) return 0; return estimate_values_.begin()->second.size(); }

private:
  // methods
  Manager() { };

  // members
  vector<CreatorPtr>              creators_;
  Map<string, vector<Double>>     estimate_values_;
};

} /* namespace estimates */
} /* namespace niwa */
#endif /* ESTIMATE_MANAGER_H_ */
