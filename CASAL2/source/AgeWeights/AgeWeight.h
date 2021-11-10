/**
 * @file AgeWeight.h
 * @author  C.Marsh
 * @date 16/11/2017
 * @section LICENSE
 *
 * Copyright NIWA Science �2017 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * AgeWeight classes allows users to ignore length completely in age based models
 */
#ifndef AGEWEIGHT_H_
#define AGEWEIGHT_H_

// headers
#include "../BaseClasses/Object.h"
#include "../Partition/Category.h"

// namespaces
namespace niwa {
class Model;

// classes
class AgeWeight : public niwa::base::Object {
public:
  // methods
  AgeWeight() = delete;
  explicit AgeWeight(shared_ptr<Model> model);
  virtual ~AgeWeight(){};
  void         Validate();
  void         Build();
  void         Verify(shared_ptr<Model> model){};
  void         Reset();
  virtual void RebuildCache();

  // accessor
  virtual Double mean_weight_at_age_by_year(unsigned year, unsigned age) = 0;

protected:
  // methods

  virtual void DoValidate()     = 0;
  virtual void DoBuild()        = 0;
  virtual void DoReset()        = 0;
  virtual void DoRebuildCache() = 0;
  // members
  shared_ptr<Model> model_ = nullptr;
};

} /* namespace niwa */
#endif /* AGEWEIGHT_H_ */
