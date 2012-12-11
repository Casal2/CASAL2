/**
 * @file Finder.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 11/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is responsible for finding categories based on a tag
 * when combined with the format string
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef FINDER_H_
#define FINDER_H_

namespace isam {
namespace categories {

class Finder {
public:
  Finder();
  virtual ~Finder();
};

} /* namespace categories */
} /* namespace isam */
#endif /* FINDER_H_ */
