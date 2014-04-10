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
namespace isam {
namespace profiles {

using std::string;

/**
 * class definition
 */
class Factory: public isam::base::Factory<isam::Profile, isam::profiles::Manager> { };

} /* namespace profiles */
} /* namespace isam */
#endif /* PROFILES_FACTORY_H_ */
