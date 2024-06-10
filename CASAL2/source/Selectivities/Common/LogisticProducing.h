/**
 * @file LogisticProducing.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef LOGISTICPRODUCING_H_
#define LOGISTICPRODUCING_H_

// Headers
#include "../../Selectivities/Selectivity.h"

// namespaces
namespace niwa {
namespace selectivities {

/**
 * Class definition
 */
class LogisticProducing : public niwa::Selectivity {
public:
  // Methods
  explicit LogisticProducing(shared_ptr<Model> model);
  virtual ~LogisticProducing() = default;
  void DoValidate() override final;

  Double get_value(Double value)  override final;  
  Double get_value(unsigned value)  override final; 
  

protected:
  // Methods
private:
  // Members
  unsigned low_;
  unsigned high_;
  Double   a50_;
  Double   ato95_;
  Double   alpha_;
};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* LOGISTICPRODUCING_H_ */
