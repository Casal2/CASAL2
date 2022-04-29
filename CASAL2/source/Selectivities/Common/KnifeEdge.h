/**
 * @file KnifeEdge.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef KNIFEEDGE_H_
#define KNIFEEDGE_H_

// Headers
#include "../../Selectivities/Selectivity.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Class Definition
 */
class KnifeEdge : public niwa::Selectivity {
public:
  // Methods
  explicit KnifeEdge(shared_ptr<Model> model);
  virtual ~KnifeEdge() = default;
  void DoValidate() override final{};

  Double get_value(Double value)  override final;
  Double get_value(unsigned value)  override final; 
 
private:
  // Members
  Double edge_;
  Double alpha_;
};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* KNIFEEDGE_H_ */
