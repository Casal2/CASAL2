/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/03/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifndef PROFILES_FACTORY_H_
#define PROFILES_FACTORY_H_

// headers
#include <string>

#include "BaseClasses/Factory.h"
#include "Profiles/Manager.h"
#include "Profiles/Profile.h"

// namespaces
namespace niwa {
namespace profiles {

using std::string;

/**
 * class definition
 */
class Factory: public niwa::base::Factory<niwa::Profile, niwa::profiles::Manager> { };

} /* namespace profiles */
} /* namespace niwa */
#endif /* PROFILES_FACTORY_H_ */
