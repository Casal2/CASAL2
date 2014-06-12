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
 * << Add Description >>
 */
#ifndef PROJECT_H_
#define PROJECT_H_

// headers
#include <boost/shared_ptr.hpp>

#include "BaseClasses/Object.h"

// namespaces
namespace isam {

class Project;
typedef void (Project::*UpdateFunction)(Double);

/**
 * Class definition
 */
class Project : public isam::base::Object {
public:
  // methods
  Project();
  virtual                     ~Project() = default;
  void                        Validate();
  void                        Build();
  void                        Reset() { DoReset(); };

  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoReset() = 0;

protected:
  // methods
  void                        SetSingleValue(Double value) {}
  void                        SetVectorValue(Double value) {}
  void                        SetMapValue(Double value) {}

  // function pointers
  UpdateFunction              DoUpdateFunc_;

  // members
  string                      type_ = "";
  vector<unsigned>            years_;
  string                      parameter_;
};

/**
 * Typedef
 */
typedef boost::shared_ptr<Project> ProjectPtr;

} /* namespace isam */

#endif /* PROJECT_H_ */
