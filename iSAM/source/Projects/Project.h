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
  Project();
  virtual                     ~Project() = default;
  void                        Validate();
  void                        Build();
  void                        Reset() { DoReset(); };
  void                        Update(unsigned current_year);

protected:
  // methods
  void                        RestoreOriginalValue();
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
  ModelPtr                    model_;
  string                      type_ = "";
  vector<unsigned>            years_;
  string                      parameter_;
  Double                      original_value_ = 0;
  map<unsigned, Double>*      estimable_map_ = 0;
  vector<Double>*             estimable_vector_ = 0;
  Double*                     estimable_ = 0;
};

/**
 * Typedef
 */
typedef std::shared_ptr<Project> ProjectPtr;

} /* namespace niwa */

#endif /* PROJECT_H_ */
