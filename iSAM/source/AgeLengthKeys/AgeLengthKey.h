/**
 * @file AgeLengthKey.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 12/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Age length keys provide a lookup matrix to
 */
#ifndef SOURCE_AGELENGTHKEYS_AGELENGTHKEY_H_
#define SOURCE_AGELENGTHKEYS_AGELENGTHKEY_H_

// headers
#include <memory>

#include "BaseClasses/Object.h"
#include "Partition/Category.h"

// namespaces
namespace niwa {

// classes
class AgeLengthKey : public base::Object {
public:
  // methods
  AgeLengthKey();
  virtual                     ~AgeLengthKey() = default;
  void                        Validate();
  void                        Build();
  void                        Reset() { DoReset(); };

  virtual void                DoAgeToLengthConversion(std::shared_ptr<partition::Category> category) = 0;

protected:
  // methods
  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoReset() = 0;

  // members
  map<unsigned,map<unsigned, Double>> lookup_table_; // map<age, map<length_index, proportion>>
};

// typedef
typedef std::shared_ptr<AgeLengthKey> AgeLengthKeyPtr;

} /* namespace niwa */

#endif /* SOURCE_AGELENGTHKEYS_AGELENGTHKEY_H_ */
