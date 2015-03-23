/**
 * @file Bindable.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/10/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is a specialisation of our parameter class that is used when
 * binding parameters to configuration file values for objects.
 */
#ifndef PARAMETERS_BINDABLE_H_
#define PARAMETERS_BINDABLE_H_

// headers
#include <typeinfo>
#include <vector>

#include "ParameterList/Parameter.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa {
namespace parameters {

using std::vector;

/**
 * class definition
 */
template<typename T>
class Bindable : public niwa::parameterlist::Parameter {
public:
  // methods
  Bindable(const string& label, T* target, const string& description);
  virtual                     ~Bindable() = default;
  void                        Bind() override final;

  // acessors
  void                        set_default_value(T value) { default_value_ = value; }
  string                      stored_type() const override final { return utilities::demangle(typeid(*target_).name()); }
  vector<string>              current_values() override final;
  void                        set_allowed_values(std::initializer_list<T> list);
  void                        set_range(T lower_bound, T upper_bound, bool lower_inclusive = true, bool upper_inclusive = true);
  void                        set_lower_bound(T lower_bound, bool inclusive = true);
  void                        set_upper_bound(T upper_bound, bool inclusive = true);

private:
  // class
  struct Range {
    bool lower_flagged_ = false;
    bool upper_flagged_ = false;
    T lower_bound_;
    T upper_bound_;
    bool lower_inclusive_;
    bool upper_inclusive_;
  };

  // members
  T*                          target_;
  T                           default_value_;
  vector<T>                   allowed_values_;
  Bindable::Range             range_;
};

} /* namespace parameters */
} /* namespace niwa */

// headers
#include "Bindable-inl.h"

#endif /* BINDABLE_H_ */
