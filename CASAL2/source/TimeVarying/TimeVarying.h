/**
 * @file TimeVarying.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 27/01/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef TIMEVARYING_H_
#define TIMEVARYING_H_

// headers
#include "../BaseClasses/Object.h"
#include "../Model/Model.h"

// namespaces
namespace niwa {

/**
 *
 */
class TimeVarying : public niwa::base::Object {
  typedef void (TimeVarying::*UpdateFunction)(Double);

public:
  // methods
  TimeVarying() = delete;
  explicit TimeVarying(shared_ptr<Model> model);
  virtual ~TimeVarying() = default;
  void Validate();
  void Build();
  void Verify(shared_ptr<Model> model);
  void Reset();
  void Update(unsigned current_year);
  void RestoreOriginalValue();

  // accessors
  vector<unsigned>&      get_years() { return years_; }
  map<unsigned, Double>& get_parameter_by_year() { return parameter_by_year_; }
  string&                get_target_parameter_label() { return parameter_; }

protected:
  // methods

  // settors
  void set_single_value(Double value);
  void set_vector_value(Double value);
  void set_map_value(Double value);
  void set_string_map_value(Double value);

  // pure virtual methods
  virtual void DoValidate() = 0;
  virtual void DoBuild()    = 0;
  virtual void DoReset()    = 0;
  virtual void DoUpdate()   = 0;

  // function pointers
  UpdateFunction update_function_ = 0;

  // members
  shared_ptr<Model>           model_           = nullptr;
  base::Object*               target_object_   = nullptr;
  string                      type_            = "";
  bool                        IsEstimableType_ = false;
  vector<unsigned>            years_;
  string                      parameter_             = "";
  Double                      original_value_        = 0;
  map<unsigned, Double>*      addressable_map_       = 0;  // unsigned map
  vector<Double>*             addressable_vector_    = 0;
  Double*                     addressable_           = 0;
  OrderedMap<string, Double>* addressable_sting_map_ = 0;
  map<unsigned, Double>       parameter_by_year_;
  string                      string_map_key_;
};

typedef std::shared_ptr<TimeVarying> TimeVaryingPtr;

} /* namespace niwa */

#endif /* TIMEVARYING_H_ */
