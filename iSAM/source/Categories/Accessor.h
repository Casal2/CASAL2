/**
 * @file Accessor.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is the base class for a category accessor. It's
 * responsible for providing the basic functionality to be useful.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef ACCESSOR_H_
#define ACCESSOR_H_

// Headers
#include <boost/shared_ptr.hpp>

// Namespaces
namespace isam {
namespace categories {

/**
 * Class Definition
 */
class Accessor {
public:
  Accessor();
  virtual ~Accessor();
};

typedef boost::shared_ptr<isam::categories::Accessor> AccessorPtry;

} /* namespace categories */
} /* namespace isam */
#endif /* ACCESSOR_H_ */
