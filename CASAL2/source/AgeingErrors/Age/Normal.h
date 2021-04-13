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
 */
#ifndef AGEINGERRORS_NORMAL_H_
#define AGEINGERRORS_NORMAL_H_

// headers
#include "../../AgeingErrors/AgeingError.h"

// mamespaces
namespace niwa {
namespace ageingerrors {

// classes
class Normal : public niwa::AgeingError {
public:
  // methods
  Normal(shared_ptr<Model> model);
  virtual                     ~Normal() = default;

protected:
  // methods
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;

private:
  // members
  Double                      cv_ = 0.0;
  unsigned                    k_;
};

} /* namespace ageingerrors */
} /* namespace niwa */
#endif /* AGEINGERRORS_NORMAL_H_ */
