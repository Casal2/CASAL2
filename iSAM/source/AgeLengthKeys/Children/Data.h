/**
 * @file Data.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 12/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * TODO: Add documentation
 */
#ifndef SOURCE_AGELENGTHKEYS_CHILDREN_DATA_H_
#define SOURCE_AGELENGTHKEYS_CHILDREN_DATA_H_

// headers
#include "AgeLengthKeys/AgeLengthKey.h"

// namespaces
namespace niwa {
namespace agelengthkeys {

// classes
class Data : public AgeLengthKey {
public:
  // methods
  Data();
  virtual                     ~Data() = default;
  void                        DoAgeToLengthConversion(std::shared_ptr<partition::Category> category) override final { };

private:
  // methods
  void                        DoValidate() override final {};
  void                        DoBuild() override final;
  void                        DoReset() override final {};

  // members
  parameters::TablePtr        data_table_;
};

} /* namespace agelengthkeys */
} /* namespace niwa */

#endif /* SOURCE_AGELENGTHKEYS_CHILDREN_DATA_H_ */
