/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This is a standard manager for our minisers
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef MINIMISERS_MANAGER_H_
#define MINIMISERS_MANAGER_H_

// Headers
#include <mutex>

#include "../BaseClasses/Manager.h"
#include "../Minimisers/Minimiser.h"

// Namespaces
namespace niwa {
class Runner;
namespace minimisers {

/**
 * Class Definition
 */
class Manager : public niwa::base::Manager<minimisers::Manager, niwa::Minimiser> {
  friend class niwa::base::Manager<minimisers::Manager, niwa::Minimiser>;
  friend class niwa::Managers;
  friend class niwa::Runner;

public:
  // Methods
  virtual ~Manager() noexcept(true);
  void Validate() override final;
  void Validate(shared_ptr<Model> model);
  void Build() final;

  // Accessors
  Minimiser* active_minimiser() { return active_minimiser_; }

protected:
  // methods
  Manager();

private:
  // members
  Minimiser*        active_minimiser_ = nullptr;
  static std::mutex lock_;
  bool              has_validated_ = false;
  bool              has_built_     = false;
};

} /* namespace minimisers */
} /* namespace niwa */
#endif /* MANAGER_H_ */
