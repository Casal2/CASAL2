/**
 * @file Model.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
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
#include "BaseClasses/Executor.h"
#include "BaseClasses/Object.h"
#include "GlobalConfiguration/GlobalConfiguration.h"
#include "Utilities/RunMode.h"

// Namespaces
namespace niwa {

using std::shared_ptr;
using base::Executor;
class Managers;
class Objects;
class Categories;
class Factory;
class Partition;
class ObjectiveFunction;

namespace State {
enum Type {
  kStartUp, // system is loading from configuration file
  kValidate, // validating user supplied values for variables
  kBuild, // building and checking relationships between objects
  kVerify, // verifying business rules (not yet implemented)
  kInitialise, // running through the initialisation phases
  kPreExecute, // called at start of an iteration
  kExecute, // execute the object
  kPostExecute, // called at the end of an iteration
  kIterationComplete, // a single iteration of the model is complete
  kReset, // called between iterations to ensure objects caches are reset
  kInputIterationComplete, // a single run of the mode is complete using an input file to set estimables
  kFinalise // the model is finished
};
}

enum class PartitionStructure {
  kAge = 1,
  kLength = 2,
  kAny = 4,
  kInvalid = 4096
};

inline PartitionStructure operator&(PartitionStructure a, PartitionStructure b) {
  return static_cast<PartitionStructure>(static_cast<int>(a) & static_cast<int>(b));
}

/**
 * Class definition
 */
class Model : public base::Object {
public:
  // Methods
  Model();
  virtual                     ~Model();
  bool                        Start(RunMode::Type run_mode);
  void                        FullIteration();
  void                        Subscribe(State::Type state, Executor* executor) { executors_[state].push_back(executor); }


  // Accessors
  RunMode::Type               run_mode() const { return run_mode_; }
  State::Type                 state() const { return state_; }
  virtual unsigned            start_year() const { return start_year_; }
  virtual unsigned            final_year() const { return final_year_; }
  unsigned                    projection_final_year() const { return projection_final_year_; }
  Double                      b0(string derived_quantity_label) {return b0_[derived_quantity_label];}
  void                        set_b0(string derived_quantity_label, Double new_b0) {b0_[derived_quantity_label] = new_b0;}
  Double                      binitial(string derived_quantity_label) {return binitial_[derived_quantity_label];}
  void                        set_binitial(string derived_quantity_label, Double new_binitial) {binitial_[derived_quantity_label] = new_binitial;}
  virtual vector<unsigned>    years() const;
  unsigned                    year_spread() const;
  virtual unsigned            current_year() const { return current_year_; }
  virtual unsigned            min_age() const { return min_age_; }
  virtual unsigned            max_age() const { return max_age_; }
  virtual unsigned            age_spread() const { return (max_age_ - min_age_) + 1; }
  virtual bool                age_plus() const { return age_plus_; }
  virtual const vector<string>&       time_steps() const { return time_steps_; }
  const vector<string>&       initialisation_phases() const { return initialisation_phases_; }
  PartitionStructure          partition_structure() const { return partition_structure_; }
  const vector<Double>        length_bins() const { return length_bins_; }

  // manager accessors
  virtual Managers&           managers();
  virtual Objects&            objects();
  GlobalConfiguration&        global_configuration() { return *global_configuration_; }
  virtual Categories*         categories() { return categories_; }
  virtual Factory&            factory();
  virtual Partition&          partition();
  virtual ObjectiveFunction&  objective_function();

protected:
  // Methods
  void                        Validate();
  void                        Build();
  void                        Verify();
  void                        Iterate();
  void                        Reset();
  void                        RunBasic();
  void                        RunEstimation();
  bool                        RunMCMC();
  void                        RunProfiling();
  void                        RunSimulation();
  void                        RunProjection();

  // Members
  RunMode::Type               run_mode_ = RunMode::kInvalid;
  State::Type                 state_    = State::kStartUp;
  unsigned                    start_year_ = 0;
  unsigned                    final_year_ = 0;
  unsigned                    projection_final_year_ = 0;
  unsigned                    current_year_ = 0;
  unsigned                    min_age_ = 0;
  unsigned                    max_age_ = 0;
  map<string, Double>         b0_;
  map<string, Double>         binitial_;
  bool                        age_plus_ = true;
  vector<string>              initialisation_phases_;
  vector<string>              time_steps_;
  vector<Double>              length_bins_;
  bool                        estimable_values_file_ = false;
  unsigned                    estimable_values_count_ = 1;
  PartitionStructure          partition_structure_ = PartitionStructure::kInvalid;
  Managers*                   managers_ = nullptr;
  Objects*                    objects_ = nullptr;
  GlobalConfiguration*        global_configuration_ = nullptr;
  Categories*                 categories_ = nullptr;
  Factory*                    factory_ = nullptr;
  Partition*                  partition_ = nullptr;
  ObjectiveFunction*          objective_function_ = nullptr;
  map<State::Type, vector<Executor*>> executors_;
};

} /* namespace niwa */
#endif /* MODEL_H_ */
