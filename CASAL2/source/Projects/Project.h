/**
 * @file Project.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * A project(ion) value is an addressable in the system that has it's value changed
 * dynamically through future models of the year.
 *
 * This object supports 3 different types of addressables
 * 1. A single Double
 * 2. A vector of doubles (where it will add a value to the end)
 * 3. A map indexed by unsigned (years)
 */
#ifndef PROJECT_H_
#define PROJECT_H_

// headers
#include "../BaseClasses/Object.h"
#include "../Model/Model.h"

// namespaces
namespace niwa {

/**
 * Class definition
 */
class Project : public niwa::base::Object {
  typedef void (Project::*UpdateFunction)(Double);

public:
  // methods
  Project() = delete;
  explicit Project(shared_ptr<Model> model);
  virtual ~Project() = default;
  void Validate();
  void Build();
  void Reset();
  void Update(unsigned current_year);
  void StoreValue(unsigned current_year);

  // accessors
  string                 parameter() { return parameter_; };
  map<unsigned, Double>& projected_parameters() { return projected_values_; };

protected:
  // methods
  void RestoreOriginalValue(unsigned year);
  void SetSingleValue(Double value);
  void SetVectorValue(Double value);
  void SetMapValue(Double value);

  // pure virtual methods
  virtual void DoValidate() = 0;
  virtual void DoBuild()    = 0;
  virtual void DoReset()    = 0;
  virtual void DoUpdate()   = 0;

  // function pointers
  UpdateFunction DoUpdateFunc_ = nullptr;

  // members
  shared_ptr<Model>      model_;
  Double                 multiplier_;
  string                 type_ = "";
  vector<unsigned>       years_;
  string                 parameter_;
  Double                 original_value_     = 0;
  Double*                addressable_        = nullptr;
  map<unsigned, Double>* addressable_map_    = nullptr;
  vector<Double>*        addressable_vector_ = nullptr;
  map<unsigned, Double>  projected_values_;
  map<unsigned, Double>  stored_values_;
  bool                   final_phase_ = false;

private:
  bool ycs_value_check_;
};
} /* namespace niwa */

#endif /* PROJECT_H_ */
