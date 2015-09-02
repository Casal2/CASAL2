/**
 * @file OffByOne.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 19/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * TODO: Add documentation
 * TODO: Implement this class
 */
#ifndef AGEINGERRORS_OFFBYONE_H_
#define AGEINGERRORS_OFFBYONE_H_

// headers
#include "AgeingErrors/AgeingError.h"

// namespaces
namespace niwa {
namespace ageingerrors {

// classes
class OffByOne : public niwa::AgeingError {
public:
  // methods
  OffByOne();
  virtual                   ~OffByOne() = default;

protected:
  // methods
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;

private:
  // members
  Double                      p1_ = 0.0;
  Double                      p2_ = 0.0;
  unsigned                    k_;
};

} /* namespace ageingerrors */
} /* namespace niwa */
#endif /* AGEINGERRORS_OFFBYONE_H_ */
