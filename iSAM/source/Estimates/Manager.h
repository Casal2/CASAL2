/*
 * Manager.h
 *
 *  Created on: 14/01/2013
 *      Author: Admin
 */

#ifndef ESTIMATE_MANAGER_H_
#define ESTIMATE_MANAGER_H_

#include "BaseClasses/Manager.h"
#include "Estimates/Estimate.h"
#include "Estimates/Info.h"

namespace isam {
namespace estimates {

class Manager : public isam::base::Manager<estimates::Manager, isam::Estimate> {
public:
  Manager();
  virtual                         ~Manager() noexcept(true);
  void                            AddObject(EstimatePtr object) override final { objects_.push_back(object); }
  void                            AddObject(EstimateInfoPtr object) { estimate_infos_.push_back(object); }
  void                            Validate() override final;
  unsigned                        GetEnabledCount();
  vector<EstimatePtr>             GetEnabled();
  void                            RemoveAllObjects() override final;
  bool                            HasEstimate(const string& parameter);
  void                            EnableEstimate(const string& parameter);
  void                            DisableEstimate(const string& parameter);

private:
  vector<EstimateInfoPtr>         estimate_infos_;

};

} /* namespace estimates */
} /* namespace isam */
#endif /* ESTIMATE_MANAGER_H_ */
