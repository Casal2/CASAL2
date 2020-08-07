/**
 * @file BindableVector.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/10/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class i
 */
#ifndef PARAMETERS_BINDABLEVECTOR_H_
#define PARAMETERS_BINDABLEVECTOR_H_

// headers
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
class BindableVector : public niwa::parameterlist::Parameter {
public:
  // methods
  BindableVector() = delete;
  BindableVector(const string& label, vector<T>* target, const string& description);
  virtual                     ~BindableVector() = default;
  void                        Bind() override final;

  // accessors
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
  vector<T>*                  target_ = nullptr;
  vector<T>                   allowed_values_;
  Range                       range_;
};

} /* namespace parameters */
} /* namespace niwa */

// headers
#include "BindableVector-inl.h"

#endif /* BINDABLEVECTOR_H_ */
