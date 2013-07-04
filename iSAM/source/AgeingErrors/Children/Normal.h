/**
 * @file Normal.h
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
#ifndef NORMAL_H_
#define NORMAL_H_

// Headers
#include "AgeingErrors/AgeingError.h"

// Namespaces
namespace isam {
namespace ageingerrors {

/**
 * Class Definition
 */
class Normal : public isam::AgeingError {
public:
  // Methods
  Normal();
  virtual                     ~Normal() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;

private:
  // Members
  Double                      cv_;
  unsigned                    k_;
};

} /* namespace ageingerrors */
} /* namespace isam */
#endif /* NORMAL_H_ */
