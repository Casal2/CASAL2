/**
 * @file Model.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is the primary representation of our model and it's states
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef MODEL_H_
#define MODEL_H_

// Headers
#include <memory>

#include "../BaseClasses/Executor.h"
#include "../BaseClasses/Object.h"
#include "../GlobalConfiguration/GlobalConfiguration.h"
#include "../Utilities/Math.h"
#include "../Utilities/PartitionType.h"
#include "../Utilities/RunMode.h"

// Namespaces
namespace niwa {
using base::Executor;
class Managers;
class Objects;
class Categories;
class Factory;
class Partition;
class ObjectiveFunction;
class EquationParser;

namespace State {
enum Type {
  kStartUp,                 // system is loading from configuration file
  kValidate,                // validating user supplied values for variables
  kBuild,                   // building and checking relationships between objects
  kVerify,                  // verifying business rules (not yet implemented)
  kInitialise,              // running through the initialisation phases
  kExecute,                 // execute the object
  kIterationComplete,       // a single iteration of the model is complete
  kReset,                   // called between iterations to ensure objects caches are reset
  kInputIterationComplete,  // a single run of the model is complete using an input file to set estimables
  kFinalise                 // the model is finished
};
}

namespace Units {
enum Type { kGrams, kKilograms, kTonnes };
} /* namespace Units */

/**
 * Class definition
 */
class Model : public base::Object, public std::enable_shared_from_this<Model> {
public:
  // Methods
  Model();
  virtual ~Model();
  virtual bool PrepareForIterations();
  virtual bool Start(RunMode::Type run_mode);
  virtual void FullIteration();
  void         Finalise();
  void         Subscribe(State::Type state, Executor* executor) { executors_[state].push_back(executor); }
  void         PopulateParameters();

  // Accessors
  RunMode::Type                 run_mode() const { return run_mode_; }
  State::Type                   state() const { return state_; }
  virtual unsigned              start_year() const { return start_year_; }
  virtual unsigned              final_year() const { return final_year_; }
  unsigned                      projection_final_year() const { return projection_final_year_; }
  Double                        b0(string derived_quantity_label) { return b0_[derived_quantity_label]; }
  void                          set_b0(string derived_quantity_label, Double new_b0) { b0_[derived_quantity_label] = new_b0; }
  Double                        binitial(string derived_quantity_label) { return binitial_[derived_quantity_label]; }
  void                          set_binitial(string derived_quantity_label, Double new_binitial) { binitial_[derived_quantity_label] = new_binitial; }
  bool                          b0_initialised(string derived_quantity_label) { return b0_initialised_[derived_quantity_label]; }
  void                          set_b0_initialised(string derived_quantity_label, bool new_b0_initialised) { b0_initialised_[derived_quantity_label] = new_b0_initialised; }
  bool                          projection_final_phase() { return projection_final_phase_; }
  void                          set_projection_final_phase(bool phase) { projection_final_phase_ = phase; }
  virtual vector<unsigned>      years() const;
  virtual vector<unsigned>      years_all() const;
  unsigned                      year_spread() const;
  virtual unsigned              current_year() const { return current_year_; }
  virtual string                base_weight_units() const { return base_weight_units_; }
  virtual unsigned              min_age() const { return min_age_; }
  virtual unsigned              max_age() const { return max_age_; }
  virtual unsigned              age_spread() const { return (max_age_ - min_age_) + 1; }
  virtual bool                  age_plus() const { return age_plus_; }
  virtual const vector<string>& time_steps() const { return time_steps_; }
  virtual const vector<string>& initialisation_phases() const { return initialisation_phases_; }
  void                          set_partition_type(PartitionType partition_type) { partition_type_ = partition_type; }
  virtual PartitionType         partition_type() const { return partition_type_; }
  virtual const vector<double>& length_bins() const { return model_length_bins_; }
  virtual bool                  length_plus() const { return length_plus_; }
  virtual const vector<double>& length_bin_mid_points() const { return model_length_bin_mid_points_; }
  virtual double                length_plus_group() const { return length_plus_group_; }
  unsigned                      get_number_of_length_bins() const { return number_of_model_length_bins_; }
  bool                          are_length_bin_compatible_with_model_length_bins(vector<double>& length_bins);
  vector<int>                   get_map_for_bespoke_length_bins_to_global_length_bins(vector<double> length_bins, bool plus_group);
  unsigned                      get_length_bin_ndx(Double value);
  void                          set_id(unsigned id) { id_ = id; }
  unsigned                      id() const { return id_; }
  void                          flag_primary_thread_model() { is_primary_thread_model_ = true; }
  bool                          is_primary_thread_model() const { return is_primary_thread_model_; }
  unsigned                      threads() const { return threads_; }
  bool                          addressables_value_file() const { return addressable_values_file_; }
  void                          set_run_mode(RunMode::Type run_mode) { run_mode_ = run_mode; }

  void set_global_configuration(GlobalConfiguration* value) { global_configuration_ = value; }

  // manager accessors
  virtual shared_ptr<Managers> managers();
  virtual Objects&             objects();
  GlobalConfiguration&         global_configuration();
  virtual Categories*          categories();
  virtual Factory&             factory();
  virtual Partition&           partition();
  virtual ObjectiveFunction&   objective_function();
  EquationParser&              equation_parser();
  shared_ptr<Model>            pointer() { return shared_from_this(); }

protected:
  // Methods
  void Validate();
  void Build();
  void Verify();
  void Iterate();
  void Reset();
  void RunBasic();
  void RunEstimation();  // important!!! if you change this method you need to change it in Runner.cpp
  bool RunMCMC();        // important!!! if you change this method you need to change it in Runner.cpp
  void RunProfiling();
  void RunSimulation();
  void RunProjection();

  virtual void DoValidate(){};

  // Members
  unsigned             id_                          = 0;
  bool                 is_primary_thread_model_     = false;
  unsigned             threads_                     = 1;
  RunMode::Type        run_mode_                    = RunMode::kInvalid;
  State::Type          state_                       = State::kStartUp;
  unsigned             start_year_                  = 0;
  unsigned             final_year_                  = 0;
  unsigned             projection_final_year_       = 0;
  unsigned             current_year_                = 0;
  unsigned             min_age_                     = 0;
  unsigned             max_age_                     = 0;
  string               base_weight_units_           = "";
  unsigned             number_of_model_length_bins_ = 0;
  map<string, Double>  b0_;
  map<string, Double>  binitial_;
  map<string, bool>    b0_initialised_;
  bool                 age_plus_ = true;
  vector<string>       initialisation_phases_;
  vector<string>       time_steps_;
  vector<double>       model_length_bins_;
  vector<double>       model_length_bin_mid_points_;
  bool                 length_plus_              = true;
  double               length_plus_group_        = 0;
  bool                 addressable_values_file_  = false;
  unsigned             addressable_values_count_ = 1;
  PartitionType        partition_type_           = PartitionType::kInvalid;
  shared_ptr<Managers> managers_;
  Objects*             objects_                = nullptr;
  GlobalConfiguration* global_configuration_   = nullptr;
  Categories*          categories_             = nullptr;
  Factory*             factory_                = nullptr;
  Partition*           partition_              = nullptr;
  ObjectiveFunction*   objective_function_     = nullptr;
  EquationParser*      equation_parser_        = nullptr;
  bool                 projection_final_phase_ = false;  // this parameter is for the projection classes. most of the methods are in the reset but they don't need to be applied
  // if the model is in the first iteration and storeing values.
  map<State::Type, vector<Executor*>> executors_;
};

} /* namespace niwa */
#endif /* MODEL_H_ */
