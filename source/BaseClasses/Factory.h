/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/10/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is a base template factory class that allows
 * the easy creation of new objects based on their parent type
 * and a string-type parameter
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef FACTORY_H_
#define FACTORY_H_

// namespaces
namespace iSAM {
namespace Base {

/**
 * Class definition
 */
class Factory {
private:
  // Methods
  Factory() { };
  virtual ~Factory() { };
};

} /* namespace Base */
} /* namespace iSAM */
#endif /* FACTORY_H_ */
