/**
 * @file Model.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Model.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include "Partition/Accessors/All.h"

#include "Factory.h"
#include "Managers.h"
#include "Objects.h"
#include "Categories/Categories.h"
#include "ConfigurationLoader/EstimableValuesLoader.h"
#include "ConfigurationLoader/MCMCObjective.h"
#include "ConfigurationLoader/MCMCSample.h"
#include "EquationParser/EquationParser.h"
#include "Estimables/Estimables.h"
#include "Estimates/Manager.h"
#include "GlobalConfiguration/GlobalConfiguration.h"
#include "InitialisationPhases/Manager.h"
#include "Logging/Logging.h"
#include "MCMCs/Manager.h"
#include "Minimisers/Manager.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Observations/Manager.h"
#include "Partition/Accessors/Category.h"
#include "Partition/Partition.h"
#include "Profiles/Manager.h"
#include "Projects/Manager.h"
#include "Reports/Manager.h"
#include "Simulates/Manager.h"
#include "TimeSteps/Manager.h"
#include "TimeVarying/Manager.h"
#include "Utilities/RandomNumberGenerator.h"
#include "Utilities/To.h"

// Namespaces
namespace niwa {

using std::cout;
using std::endl;

/**
 * Default Constructor
 */
Model::Model() {
  LOG_TRACE();
  parameters_.Bind<unsigned>(PARAM_START_YEAR, &start_year_, "Define the first year of the model, immediately following initialisation", R"(Defines the first year of the model, $\ge 1$, e.g. 1990)");
  parameters_.Bind<unsigned>(PARAM_FINAL_YEAR, &final_year_, "Define the final year of the model, excluding years in the projection period", "Defines the last year of the model, i.e., the model is run from start_year to final_year");
  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "Minimum age of individuals in the population", R"($0 \le$ age\textlow{min} $\le$ age\textlow{max})", 0);
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "Maximum age of individuals in the population", R"($0 \le$ age\textlow{min} $\le$ age\textlow{max})", 0);
  parameters_.Bind<bool>(PARAM_AGE_PLUS, &age_plus_, "Define the oldest age or extra length midpoint (plus group size) as a plus group", "true, false", false);
  parameters_.Bind<string>(PARAM_INITIALISATION_PHASES, &initialisation_phases_, "Define the labels of the phases of the initialisation", R"(A list of valid labels defined by \texttt{@initialisation_phase})", true);
  parameters_.Bind<string>(PARAM_TIME_STEPS, &time_steps_, "Define the labels of the time steps, in the order that they are applied, to form the annual cycle", R"(A list of valid labels defined by \texttt{@time_step})");
  parameters_.Bind<unsigned>(PARAM_PROJECTION_FINAL_YEAR, &projection_final_year_, "Define the final year of the model in projection mode", R"(Defines the last year of the projection period, i.e., the projection period runs from \texttt{final_year}$+1$ to \texttt{projection_final_year}. For the default, $0$, no projections are run.)", 0);
  parameters_.Bind<string>(PARAM_TYPE, &type_, "TBA: Type of model (the partition structure). Either age, length or hybrid", "", PARAM_AGE)->set_allowed_values({PARAM_AGE, PARAM_LENGTH, PARAM_HYBRID});
  parameters_.Bind<unsigned>(PARAM_LENGTH_BINS, &length_bins_, "The minimum length in each length bin", R"($0 \le$ length\textlow{min} $\le$ length\textlow{max})", true);
  parameters_.Bind<bool>(PARAM_LENGTH_PLUS, &length_plus_, "Specify whether there is a length plus group or not", "true, false", true);
  parameters_.Bind<unsigned>(PARAM_LENGTH_PLUS_GROUP, &length_plus_group_, "Mean size of length plus group", R"(length\textlow{max} $<$ length_plus_group)", 0);
  parameters_.Bind<string>(PARAM_BASE_UNITS, &base_weight_units_, "Define the units for the base weight. This will be the default unit of any weight input parameters ", "grams, kgs or tonnes", PARAM_TONNES)->set_allowed_values({PARAM_GRAMS, PARAM_TONNES,PARAM_KGS});

  global_configuration_ = new GlobalConfiguration();
  managers_ = new Managers(this);
  objects_ = new Objects(this);
  categories_ = new Categories(this);
  factory_ = new Factory(this);
  partition_ = new Partition(this);
  objective_function_ = new ObjectiveFunction(this);
  equation_parser_ = new EquationParser(this);
}

/**
 * Destructor
 */
Model::~Model() {
  delete global_configuration_;
  delete managers_;
  delete objects_;
  delete factory_;
  delete categories_;
  delete partition_;
  delete objective_function_;
}

/**
 *
 */
vector<unsigned> Model::years() const {
  vector<unsigned> years;
  unsigned year;
  for (year = start_year_; year <= final_year_; ++year)
    years.push_back(year);
  if (run_mode_ == RunMode::kProjection) {
    for (; year <= projection_final_year_; ++year)
      years.push_back(year);
  }
  return years;
}

/**
 *
 */
unsigned Model::year_spread() const {
  unsigned spread = (final_year_ - start_year_) + 1;
  if (run_mode_ == RunMode::kProjection)
    spread = (projection_final_year_ - start_year_) + 1;

  return spread;
}

/**
 *
 */
Managers& Model::managers() {
  return *managers_;
}

Objects& Model::objects() {
  return *objects_;
}

Factory& Model::factory() {
  return *factory_;
}

Partition& Model::partition() {
  return *partition_;
}

ObjectiveFunction& Model::objective_function() {
  return *objective_function_;
}

EquationParser& Model::equation_parser() {
  return *equation_parser_;
}

/**
 * Start our model. This is the entry point method for the model
 * after being called from the main() method.
 *
 * This method will start stepping through the states and verifying
 * each step.
 */
bool Model::Start(RunMode::Type run_mode) {
  LOG_TRACE();
  Logging& logging = Logging::Instance();
  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return false;
  }

  run_mode_ = run_mode;

  if (state_ != State::kStartUp)
    LOG_CODE_ERROR() << "Model state should always be startup when entering the start method";
  if (global_configuration_->estimable_value_file() != "") {
    configuration::EstimableValuesLoader loader(this);
    loader.LoadValues(global_configuration_->estimable_value_file());
  }

  managers_->report()->Execute(State::kStartUp);

  LOG_FINE() << "Model: State Change to Validate";
  state_ = State::kValidate;
  Validate();
  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return false;
  }

  managers_->report()->Execute(state_);

  LOG_FINE() << "Model: State Change to Build";
  state_ = State::kBuild;
  Build();
  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return false;
  }
  managers_->report()->Execute(state_);

  LOG_FINE() << "Model: State Change to Verify";
  state_ = State::kVerify;
  Verify();
  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return false;
  }
  managers_->report()->Execute(state_);

  // prepare all reports
  LOG_FINE() << "Preparing Reports";
  managers_->report()->Prepare();

  switch(run_mode_) {
  case RunMode::kBasic:
    RunBasic();
    break;

  case RunMode::kEstimation:
    RunEstimation();
    break;

  case RunMode::kMCMC:
    if (!RunMCMC())
      return false;
    break;

  case RunMode::kProfiling:
    RunProfiling();
    break;

  case RunMode::kSimulation:
    RunSimulation();
    break;

  case RunMode::kProjection:
    RunProjection();
    break;

  case RunMode::kTesting:
    break;

  default:
    LOG_ERROR() << "Invalid run mode has been specified. This run mode is not supported: " << run_mode_;
    break;
  }

  // finalise all reports
  LOG_FINE() << "Finalising Reports";
  state_ = State::kFinalise;
  for (auto executor : executors_[state_])
    executor->Execute();
  managers_->report()->Execute(state_);
  managers_->report()->Finalise();
  return true;
}

/**
 * Populate the loaded parameters
 */
void Model::PopulateParameters() {
  LOG_TRACE();

  // Check that we've actually defined a @model block
  if (block_type_ == "")
    LOG_ERROR() << "The @model block is missing from configuration file. This block is required.";

  /**
   * Validate the parameters
   */
  parameters_.Populate(this);
  if (type_ == PARAM_AGE)
    partition_type_ = PartitionType::kAge;
  else if (type_ == PARAM_LENGTH)
    partition_type_ = PartitionType::kLength;
//  else if (type_ == PARAM_HYBRID)
//    partition_type_ = PartitionType::kHybrid;

  if (partition_type_ == PartitionType::kAge) {
    if (start_year_ < 1000)
      LOG_ERROR_P(PARAM_START_YEAR) << " (" << start_year_ << ") cannot be less than 1000";
    if (start_year_ > final_year_)
      LOG_ERROR_P(PARAM_FINAL_YEAR) << " (" << final_year_ << ") cannot be less than start_year (" << start_year_ << ")";
    if (min_age_ > max_age_)
      LOG_ERROR_P(PARAM_MIN_AGE) << " (" << min_age_ << ") has been defined as greater than max_age (" << max_age_ << ")";

    if (run_mode_ == RunMode::kProjection) {
      if (projection_final_year_ <= start_year_ || projection_final_year_ <= final_year_) {
        LOG_ERROR_P(PARAM_PROJECTION_FINAL_YEAR) << "("
            << projection_final_year_ << ") cannot be less than or equal to start_year (" << start_year_
            << ") or final_year (" << final_year_ << ")";
      }
    }

  } else if (partition_type_ == PartitionType::kLength) {
    if (!parameters_.Get(PARAM_LENGTH_BINS)->has_been_defined())
      LOG_ERROR() << location() << "@model is missing required parameter " << PARAM_LENGTH_BINS;
    if (parameters_.Get(PARAM_MIN_AGE)->has_been_defined())
      LOG_ERROR_P(PARAM_MIN_AGE) << "cannot be defined in a length model";
    if (parameters_.Get(PARAM_MAX_AGE)->has_been_defined())
      LOG_ERROR_P(PARAM_MAX_AGE) << "cannot be defined in a length model";
//    if (parameters_.Get(PARAM_PLUS_GROUP)->has_been_defined())
//      LOG_ERROR_P(PARAM_PLUS_GROUP) << "cannot be defined in a length model"; // SAMIK - removed condition as plus_group applies for length

  } else
    LOG_ERROR() << "Partition structure " << (unsigned)partition_type_ << " not supported";

//  if (base_weight_units_label_ == PARAM_TONNES)
//    base_weight_units_ = Units::kTonnes;
//  else if (base_weight_units_label_ == PARAM_KGS)
//    base_weight_units_ = Units::kKilograms;
//  else if (base_weight_units_label_ == PARAM_GRAMS)
//    base_weight_units_ = Units::kGrams;
//  else
//    LOG_CODE_ERROR() << "Base weight units for model " << base_weight_units_label_ << " is invalid";
}

/**
 * First we will do the local validations. Then we will call validation on the other objects
 */
void Model::Validate() {
  // Check that we've actually defined a @model block
  if (block_type_ == "")
    LOG_ERROR() << "The @model block is missing from configuration file. This block is required.";

  if (!parameters_.has_been_populated())
    parameters_.Populate(this);

  // Call validation for the other objects required by the model
  categories_->Validate();
  partition_->Validate();

  managers_->Validate();

  /**
   * Do some more sanity checks
   */
  initialisationphases::Manager& init_phase_mngr = *managers_->initialisation_phase();
  for (const string& phase : initialisation_phases_) {
    if (!init_phase_mngr.IsPhaseDefined(phase))
      LOG_ERROR_P(PARAM_INITIALISATION_PHASES) << "(" << phase << ") has not been defined. Please ensure you have defined it";
  }

  timesteps::Manager& time_step_mngr = *managers_->time_step();
  for (const string time_step : time_steps_) {
    if (!time_step_mngr.GetTimeStep(time_step))
      LOG_ERROR_P(PARAM_TIME_STEPS) << "(" << time_step << ") has not been defined. Please ensure you have defined it";
  }

  /**
   * Do some simple checks
   * e.g Validate that the length_bins are strictly increasing
   */
//  for(unsigned length = 0; length < (length_bins_.size() - 1); ++length) {
//    if(length_bins_[length] < 0.0)
//    if(length_bins_[length] > length_bins_[length + 1])
//      LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Length bins must be strictly increasing " << length_bins_[length] << " is greater than " << length_bins_[length +1];
//  }
}

/**
 *
 */
void Model::Build() {
  LOG_TRACE();
  categories_->Build();
  partition_->Build();
  managers_->Build();

  Estimables& estimables = *managers_->estimables();
  if (estimables.GetValueCount() > 0) {
    addressable_values_file_ = true;
    adressable_values_count_ = estimables.GetValueCount();
  }

  if (categories()->HasAgeLengths()) {
    partition_->BuildMeanLengthData();
    if (length_bins_.size() > 0)
      partition_->BuildAgeLengthProportions();
  }
}

/**
 *
 */
void Model::Verify() {
  LOG_TRACE();
  for (auto executor : executors_[state_])
    executor->Execute();
}

/**
 *
 */
void Model::Reset() {
  LOG_TRACE();

  partition_->Reset();
  categories_->Reset();
  managers_->Reset();
}

/**
 *
 */
void Model::RunBasic() {
  LOG_TRACE();
  Estimables& estimables = *managers_->estimables();
  bool single_step = global_configuration_->single_step();
  vector<string> single_step_addressables;
  vector<Double*> estimable_targets;
  // Create an instance of all categories
  niwa::partition::accessors::All all_view(this);

  // Model is about to run
  for (unsigned i = 0; i < adressable_values_count_; ++i) {
    if (addressable_values_file_) {
      estimables.LoadValues(i);
      Reset();
    }

    /**
     * Running the model now
     */
    LOG_FINE() << "Model: State change to Execute";
    state_ = State::kInitialise;
    current_year_ = start_year_;
    // Iterate over all partition members and UpDate Mean Weight for the inital weight calculations
    for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
      (*iterator)->UpdateMeanLengthData();
    }
    initialisationphases::Manager& init_phase_manager = *managers_->initialisation_phase();
    init_phase_manager.Execute();
    managers_->report()->Execute(State::kInitialise);

    state_ = State::kExecute;

    /**
     * Handle single step now
     */
    if (single_step) {
      managers_->report()->Pause();
      cout << "Please enter a space separated list of addressable names to be used during single step" << endl;
      string line = "";
      string error = "";

      std::getline(std::cin, line);
      managers_->report()->Resume();
      boost::split(single_step_addressables, line, boost::is_any_of(" "));
      for (string addressable : single_step_addressables) {
        if (!objects().VerfiyAddressableForUse(addressable, addressable::kSingleStep, error)) {
          LOG_FATAL() << "The addressable " << addressable << " could not be verified for use in a single-step basic run. Error was " << error;
        }
        Double* target = objects().GetAddressable(addressable);
        estimable_targets.push_back(target);
      }
    }

    timesteps::Manager& time_step_manager = *managers_->time_step();
    timevarying::Manager& time_varying_manager = *managers_->time_varying();
    for (current_year_ = start_year_; current_year_ <= final_year_; ++current_year_) {
      LOG_FINE() << "Iteration year: " << current_year_;
      if (single_step) {
        managers_->report()->Pause();
        cout << "Please enter space separated values for estimables for year: " << current_year_ << endl;
        string line = "";
        std::getline(std::cin, line);
        managers_->report()->Resume();

        vector<string> temp_values;
        boost::split(temp_values, line, boost::is_any_of(" "));
        if (temp_values.size() != estimable_targets.size()) {
          LOG_FATAL() << "Number of values provided was " << temp_values.size()
              << " when we expected " << estimable_targets.size();
        }

        for (unsigned i = 0; i < temp_values.size(); ++i) {
          Double value = 0;
          if (!utilities::To<string, Double>(temp_values[i], value)) {
            LOG_FATAL() << "Value " << temp_values[i] << " for the estimable "
                << single_step_addressables[i] << " is invalid";
          }

          LOG_FINEST() << "Setting annual value for " << single_step_addressables[i] << " to " << value;
          *estimable_targets[i] = value;
        }
      }
      LOG_TRACE();
      time_varying_manager.Update(current_year_);
      LOG_FINEST() << "finishing update time varying now Update Category mean length and weight before beginning annual cycle";

      // Iterate over all partition members and UpDate Mean Weight for this year.
      for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
        (*iterator)->UpdateMeanLengthData();
      }

      time_step_manager.Execute(current_year_);
    }

    managers_->observation()->CalculateScores();

    for (auto executor : executors_[State::kExecute])
      executor->Execute();

    // Model has finished so we can run finalise.

    LOG_FINE() << "Model: State change to Iteration Complete";
    objective_function_->CalculateScore();
    managers_->report()->Execute(State::kIterationComplete);
  }
}

/**
 * Run the model in estimation mode.
 */
void Model::RunEstimation() {
  /*
   * Before running the model in estimation mode we'll do an iteration
   * as a basic model. We don't call any reports though.
   */
  LOG_FINE() << "Doing pre-estimation iteration of the model";
  Iterate();

  auto minimiser = managers_->minimiser()->active_minimiser();
  if (minimiser == nullptr)
    LOG_CODE_ERROR() << "if (minimiser == nullptr)";

  Estimables* estimables = managers_->estimables();
  map<string, Double> estimable_values;
  LOG_FINE() << "estimable values count: " << adressable_values_count_;
  for (unsigned i = 0; i < adressable_values_count_; ++i) {
    if (addressable_values_file_) {
      estimables->LoadValues(i);
      Reset();
    }

    run_mode_ = RunMode::kEstimation;
    LOG_FINE() << "Calling minimiser to begin the estimation with the " << i + 1 << "st/nd/nth set of values";
    unsigned max_iters = managers_->estimate()->GetNumberOfPhases();

    LOG_FINE() << "found iterations = " << max_iters;
    for (unsigned j = 1; j <= max_iters; ++j) {
    	LOG_MEDIUM() << "model.estimation_phase: " << j;
      managers_->estimate()->SetActivePhase(j);
      minimiser->Execute();
    }

    minimiser->BuildCovarianceMatrix();

    run_mode_ = RunMode::kBasic;
    FullIteration();
    run_mode_ = RunMode::kEstimation;

    LOG_FINE() << "Model: State change to Iteration Complete";
    managers_->report()->Execute(State::kIterationComplete);
  }
}

/**
 *
 */

bool Model::RunMCMC() {
  LOG_FINE() << "Entering the MCMC Sub-System";
  auto mcmc = managers_->mcmc()->active_mcmc();

  Logging& logging = Logging::Instance();
  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return false;
  }

  if (global_configuration_->resume()) {
    configuration::MCMCObjective objective_loader(this);
    if (!objective_loader.LoadFile(global_configuration_->mcmc_objective_file()))
      return false;

    configuration::MCMCSample sample_loader(this);
    if (!sample_loader.LoadFile(global_configuration_->mcmc_sample_file()))
      return false;

    // reset RNG seed for resume
    utilities::RandomNumberGenerator::Instance().Reset((unsigned int)time(NULL));

  } else if (!global_configuration_->skip_estimation()){
    /**
     * Note: This should only be called when running Casal2 in a standalone executable
     * as it must use the same build profit (autodiff or not) as the MCMC. When
     * using the front end application, skip_estimation will be flagged as true.
     *
     * This is because the front end handles the minimisation to generate the MPD file
     * and Covariance matrix for use by the MCMC
     */
    LOG_FINE() << "Calling minimiser to find our minimum and covariance matrix";
    auto minimiser = managers_->minimiser()->active_minimiser();
    if((minimiser->type() == PARAM_DE_SOLVER) | (minimiser->type() == PARAM_DLIB))
      LOG_ERROR() << "The minimiser type " << PARAM_DE_SOLVER << ", " << PARAM_DE_SOLVER << " does not produce a covariance matrix and so will not be viable for an MCMC run, try one of the other minimisers.";

    minimiser->Execute();
    LOG_FINE() << "Build covariance matrix";
    minimiser->BuildCovarianceMatrix();
    LOG_FINE() << "Minimisation complete. Starting MCMC";
  }
  LOG_FINE() << "Begin MCMC chain";
  mcmc->Execute();
  return true;
}

/**
 *
 */
void Model::RunProfiling() {
  Estimables& estimables = *managers_->estimables();

  map<string, Double> estimable_values;
  for (unsigned i = 0; i < adressable_values_count_; ++i) {
    if (addressable_values_file_) {
      estimables.LoadValues(i);
      Reset();
    }

    LOG_FINE() << "Doing pre-profile iteration of the model";
    Iterate();

    LOG_FINE() << "Entering the Profiling Sub-System";
    estimates::Manager& estimate_manager = *managers_->estimate();
    auto minimiser = managers_->minimiser()->active_minimiser();
    if (!minimiser)
      LOG_FATAL() << "couldn't get an active minimiser to estimate for the profile";
    vector<Profile*> profiles = managers_->profile()->objects();
    LOG_FINE() << "Working with " << profiles.size() << " profiles";
    for (auto profile : profiles) {
      LOG_FINE() << "Disabling estimate: " << profile->parameter();
      estimate_manager.UnFlagIsEstimated(profile->parameter());

      LOG_FINE() << "First-Stepping profile";
      profile->FirstStep();
      for (unsigned i = 0; i < profile->steps() + 2; ++i) {
        LOG_FINE() << "Calling minimiser to begin the estimation (profiling)";
        minimiser->Execute();
        LOG_FINE() << "Finished estimation from " << i + 1 << " steps";
        run_mode_ = RunMode::kBasic;
        FullIteration();

        LOG_FINE() << "Model: State change to Iteration Complete";
        run_mode_ = RunMode::kProfiling;
        managers_->report()->Execute(State::kIterationComplete);

        profile->NextStep();
      }
      profile->RestoreOriginalValue();

      estimate_manager.FlagIsEstimated(profile->parameter());
    }
  }
}

/**
 *
 */
void Model::RunSimulation() {
  LOG_FINE() << "Entering the Simulation Sub-System";

  Estimables* estimables = managers_->estimables();
  LOG_FINE() << "estimable values count: " << adressable_values_count_;
  if (adressable_values_count_ > 1)
    LOG_FATAL() << "Simulation mode only allows a -i file with one set of parameters.";

  if (addressable_values_file_) {
    estimables->LoadValues(0);
    Reset();
  }
  niwa::partition::accessors::All all_view(this);


  int simulation_candidates = global_configuration_->simulation_candidates();
  if (simulation_candidates < 1) {
    LOG_FATAL() << "The number of simulations specified at the command line parser must be at least one";
  }
  unsigned suffix_width          = (unsigned)floor(log10((double) simulation_candidates + 1)) + 1;
  for (int i = 0; i < simulation_candidates; ++i) {
    string report_suffix = ".";
    unsigned iteration_width = (unsigned)floor(log10(i + 1)) + 1;

    unsigned diff = suffix_width - iteration_width;
    report_suffix.append(diff,'0');
    report_suffix.append(utilities::ToInline<unsigned, string>(i + 1));
    managers_->report()->set_report_suffix(report_suffix);

    Reset();

    state_ = State::kInitialise;
    current_year_ = start_year_;
    // Iterate over all partition members and UpDate Mean Weight for the inital weight calculations
    for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
      (*iterator)->UpdateMeanLengthData();
    }

    initialisationphases::Manager& init_phase_manager = *managers_->initialisation_phase();
    init_phase_manager.Execute();
    managers_->report()->Execute(State::kInitialise);

    state_ = State::kExecute;
    timesteps::Manager& time_step_manager = *managers_->time_step();
    timevarying::Manager& time_varying_manager = *managers_->time_varying();
    for (current_year_ = start_year_; current_year_ <= final_year_; ++current_year_) {
      LOG_FINE() << "Iteration year: " << current_year_;
      time_varying_manager.Update(current_year_);
      // Iterate over all partition members and UpDate Mean Weight for the inital weight calculations
      for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
        (*iterator)->UpdateMeanLengthData();
      }
      managers_->simulate()->Update(current_year_);
      time_step_manager.Execute(current_year_);
    }

    managers_->observation()->CalculateScores();

    // Model has finished so we can run finalise.
    LOG_FINE() << "Model: State change to PostExecute";
    managers_->report()->Execute(State::kIterationComplete);

    managers_->report()->WaitForReportsToFinish();
  }
}

/**
 *
 */

void Model::RunProjection() {
  LOG_TRACE();
  int projection_candidates = global_configuration_->projection_candidates();
  if (projection_candidates < 1) {
    LOG_FATAL() << "The number of projections specified at the command line parser must be at least one";
  }
  Estimables& estimables = *managers_->estimables();
  vector<Double*> estimable_targets;
  // Create an instance of all categories
  niwa::partition::accessors::All all_view(this);

  // Model is about to run
  for (unsigned i = 0; i < adressable_values_count_; ++i) {
    for (int j = 0; j < projection_candidates; ++j) {
      LOG_FINE() << "Beginning initial model run for projections";
      projection_final_phase_ = false;
      if (addressable_values_file_) {
        LOG_FINE() << "loading input parameters";
        estimables.LoadValues(i);
        Reset();
      }

      LOG_FINE() << "Model: State change to Execute";
      state_ = State::kInitialise;
      current_year_ = start_year_;
      // Iterate over all partition members and UpDate Mean Weight for the inital weight calculations
      for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
        (*iterator)->UpdateMeanLengthData();
      }
      initialisationphases::Manager& init_phase_manager = *managers_->initialisation_phase();
      init_phase_manager.Execute();

      state_ = State::kExecute;

      timesteps::Manager& time_step_manager = *managers_->time_step();
      timevarying::Manager& time_varying_manager = *managers_->time_varying();
      projects::Manager& project_manager = *managers_->project();

      for (current_year_ = start_year_; current_year_ <= final_year_; ++current_year_) {
        LOG_FINE() << "Iteration year: " << current_year_;
        time_varying_manager.Update(current_year_);
        // Iterate over all partition members and UpDate Mean Weight for the inital weight calculations
        for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
          (*iterator)->UpdateMeanLengthData();
        }
        time_step_manager.Execute(current_year_);
        project_manager.StoreValues(current_year_);
      }

      /**
       * Running the model now
       */
      LOG_FINE() << "Entering the Projection Sub-System";
      // Reset the model
      projection_final_phase_ = true;
      Reset();
      state_ = State::kInitialise;
      current_year_ = start_year_;
      // Run the intialisation phase
      init_phase_manager.Execute();
      // Reset all parameter and re run the model
      managers_->report()->Execute(State::kInitialise);

      state_ = State::kExecute;
      LOG_FINE() << "Starting projection years";
      for (; current_year_ <= projection_final_year_; ++current_year_) {
        LOG_FINE() << "Iteration year: " << current_year_;
        // Iterate over all partition members and UpDate Mean Weight for the inital weight calculations
        for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
          (*iterator)->UpdateMeanLengthData();
        }
        project_manager.Update(current_year_);
        time_step_manager.Execute(current_year_);
      }

      // Model has finished so we can run finalise.
      LOG_FINE() << "Model: State change to PostExecute and iteration complete";
      managers_->report()->Execute(State::kIterationComplete);

      // Not sure if we need these
      //managers_->observation()->CalculateScores();
      //managers_->report()->WaitForReportsToFinish();
      //Reset();
    }
  }
  // Print the report to disk if tabular
  //managers_->report()->Finalise();
}

/**
 * This method will do a single iteration of the model. During
 * a basic run it'll only run once, but during the other run modes i.e. estiamtion and MCMC
 * it'll run multiple times.
 */
void Model::Iterate() {
  LOG_TRACE();
  // Create an instance of all categories
  niwa::partition::accessors::All all_view(this);

  state_ = State::kInitialise;
  current_year_ = start_year_;
  // Iterate over all partition members and UpDate Mean Weight for the inital weight calculations
  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    (*iterator)->UpdateMeanLengthData();
  }
  initialisationphases::Manager& init_phase_manager = *managers_->initialisation_phase();
  init_phase_manager.Execute();
  managers_->report()->Execute(State::kInitialise);

  state_ = State::kExecute;
  timesteps::Manager& time_step_manager = *managers_->time_step();
  timevarying::Manager& time_varying_manager = *managers_->time_varying();
  for (current_year_ = start_year_; current_year_ <= final_year_; ++current_year_) {
    LOG_FINE() << "Iteration year: " << current_year_;
    time_varying_manager.Update(current_year_);
    // Iterate over all partition members and UpDate Mean Weight for the inital weight calculations
    for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
      (*iterator)->UpdateMeanLengthData();
    }
    time_step_manager.Execute(current_year_);
  }

  managers_->observation()->CalculateScores();

  for (auto executor : executors_[State::kExecute])
    executor->Execute();

  current_year_ = final_year_;
}

/**
 *
 */
void Model::FullIteration() {
  Reset();
  Iterate();
}
} /* namespace niwa */
