/**
 * @file MigrationSS3.h
 * @author  S. Datta
 * @version 1.0
 * @date 2024
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2024-10-02 12:19:00 +1300 (Wed, 02 Oct 2024) $
 */
#ifndef MIGRATIONSS3_H_
#define MIGRATIONSS3_H_

// Headers
#include "../../Selectivities/Selectivity.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Class definition
 */
class MigrationSS3 : public niwa::Selectivity {
public:
  // Methods
  explicit MigrationSS3(shared_ptr<Model> model);
  virtual ~MigrationSS3() = default;
  void DoValidate() override final;
  void DoReset() override final;

  Double get_value(Double value)  override final; 
  Double get_value(unsigned value)  override final; 

private:
  // Members
  Double min_set_age_;
  Double max_set_age_;
  Double migration_rate_at_L_;
  Double migration_rate_at_H_;
  Double alt_rate_lower_switch_;
  Double alt_rate_upper_switch_;
  Double alt_rate_lower_;
  Double alt_rate_upper_;
  // transformed parameters
  Double r_;
  Double y1dash_;
  Double y2dash_;
  Double k_;
  Double yyoung_;
  Double yold_;
};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* MIGRATIONSS3_H_ */
