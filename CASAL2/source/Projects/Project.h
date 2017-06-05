/**
 * @file Project.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * A project(ion) value is an estimable in the system that has it's value changed
 * dynamically through future models of the year.
 *
 * This object supports 3 different types of Estimables
 * 1. A single double
 * 2. A vector of doubles (where it will add a value to the end)
 * 3. A map indexed by unsigned (years)
 */
#ifndef PROJECT_H_
#define PROJECT_H_

// headers
#include "BaseClasses/Object.h"
#include "Model/Model.h"

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
  explicit Project(Model* model);
  virtual                     ~Project() = default;
  void                        Validate();
  void                        Build();
  void                        Reset();
  void                        Update(unsigned current_year);
  virtual void                StoreValue(unsigned current_year, unsigned start_year, unsigned final_year);


  // accessors
  string                      estimable_parameter() {return estimable_parameter_;};
  Estimable::Type             estimate_type() {return estimable_type_;};
  map<unsigned,Double>&       projected_parameters() {return projected_parameters_;};

protected:
  // methods
  void                        RestoreOriginalValue(unsigned year);
  void                        SetSingleValue(Double value);
  void                        SetVectorValue(Double value);
  void                        SetMapValue(Double value);

  // pure virtual methods
  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoReset() = 0;
  virtual void                DoUpdate() = 0;

  // function pointers
  UpdateFunction              DoUpdateFunc_ = 0;

  // members
  Model*                      model_;
  Double                      multiplier_;
  string                      type_ = "";
  vector<unsigned>            years_;
  string                      parameter_;
  Double                      original_value_ = 0;
  map<unsigned, Double>*      estimable_map_ = 0;
  vector<Double>*             estimable_vector_ = 0;
  Double*                     estimable_ = 0;
  Estimable::Type             estimable_type_;
  base::Object*               target_;
  string                      estimable_parameter_  = "";
  map<unsigned,Double>        projected_parameters_; // year , value;
  bool                        final_phase_ = false;

private:
  bool                        ycs_value_check_;
};

/**
 * Typedef
 */
typedef std::shared_ptr<Project> ProjectPtr;

} /* namespace niwa */

#endif /* PROJECT_H_ */
