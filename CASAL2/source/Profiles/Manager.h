/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/03/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */
#ifndef PROFILES_MANAGER_H_
#define PROFILES_MANAGER_H_

// headers
#include "../BaseClasses/Manager.h"
#include "../Profiles/Profile.h"

// namespaces
namespace niwa {
class Model;
namespace profiles {

/**
 * Class definition
 */
class Manager : public niwa::base::Manager<niwa::profiles::Manager, niwa::Profile> {
  friend class niwa::base::Manager<niwa::profiles::Manager, niwa::Profile>;
  friend class niwa::Managers;

public:
  // methods
  virtual ~Manager() noexcept(true){};
  void Verify(shared_ptr<Model> model);
  Profile* GetProfile(); // should only be allowed one

protected:
  // methods
  Manager();
};

} /* namespace profiles */
} /* namespace niwa */
#endif /* PROFILES_MANAGER_H_ */
