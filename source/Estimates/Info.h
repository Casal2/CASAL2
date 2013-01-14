/**
 * @file Info.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is a the configuration file representation of the
 * estimate that we've specified. We use an intermediate stage because
 * there is not going to be a 1:1 relationship betwee the configuration
 * file block and the number of estimates we need to create for it.
 *
 * This class is responsible for creating N estimates per configuration
 * block. This class does have a 1:1 relationship to the configuration
 * file.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef INFO_H_
#define INFO_H_

namespace isam {
namespace estimates {

class Info {
public:
  Info();
  virtual ~Info();
};

} /* namespace estimates */
} /* namespace isam */
#endif /* INFO_H_ */
