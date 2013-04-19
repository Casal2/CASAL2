/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef FACTORY_H_
#define FACTORY_H_

// Headers
namespace isam {
namespace ageingerrors {

/**
 * Class Definition
 */
class Factory {
private:
  // Methods
  Factory() = delete;
  virtual                     ~Factory() = delete;
};

} /* namespace ageingerrors */
} /* namespace isam */
#endif /* FACTORY_H_ */
