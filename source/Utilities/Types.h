/**
 * @file Types.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/10/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file contains conditional types that we'll use in iSAM instead
 * of the default types to give us more flexibility.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef TYPES_H_
#define TYPES_H_

#ifdef USE_ADMB
#include <admb/fvar.hpp>
#endif

// Namespaces
namespace isam {
namespace utilities {

/**
 * double conditional depending on if we're using ADMB or not
 */
#ifdef USE_ADMB
typedef dvariable Double;

#else
typedef double Double;

#endif




} /* namespace utilities */
} /* namespace isam */



#endif /* TYPES_H_ */
