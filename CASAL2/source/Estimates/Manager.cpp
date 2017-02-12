/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 27/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Manager.h"

#include <algorithm>

#include "Estimables/Estimables.h"
#include "GlobalConfiguration/GlobalConfiguration.h"
#include "Model/Model.h"
#include "Model/Managers.h"
#include "Reports/Manager.h"
#include "Reports/Children/MPD.h"
#include "Utilities/DoubleCompare.h"

// Namespaces
namespace niwa {
namespace estimates {

using std::set;
namespace dc = utilities::doublecompare;

/**
 * Destructor
 */
Manager::~Manager() {
  for (auto creator : creators_)
    delete creator;
}
/**
 *
 */
void Manager::Validate() {
  LOG_CODE_ERROR() << "This method is not supported";
}
/**
 *
 */
void Manager::Validate(Model* model) {
  LOG_TRACE();
  /**
   * Run over our creators and get them to build the actual
   * estimates the system is going to build.
   *
   * Firstly we need to isolate any estimates that will reference
   * another estimate as these will need to be created after the others
   */
  vector<Creator*> delayed_creators;
  for (Creator* creator : creators_) {
    creator->parameters().Populate();

    if (creator->parameter().substr(0, 8) == PARAM_ESTIMATE) {
      delayed_creators.push_back(creator);
      continue;
    }

    creator->CreateEstimates();
  }

  // nothing fancy here, we don't support chain estimates past 1 level
  LOG_FINE() << delayed_creators.size() << " estimates were delayed because they reference other estimates";
  for (auto creators : delayed_creators) {
    creators->CreateEstimates();
  }

  /**
   * Validate the actual estimates now
   */
  for (auto estimate : objects_)
    estimate->Validate();

  /**
   * Remove any estimates where the bounds are the same.
   */
  unsigned count = objects_.size();
  objects_.erase(
      std::remove_if(objects_.begin(), objects_.end(),
         [](Estimate* estimate) {return dc::IsEqual(estimate->lower_bound(), estimate->upper_bound()); }
       ),
       objects_.end()
  );
  if (count != objects_.size())
    LOG_WARNING() << "Estimates were removed because of matching lower and upper bounds. Originally had " << count << " estimates, now have " << objects_.size();

  /**
   * Load any estimate values that have been supplied
   */
  GlobalConfiguration& global_config = model->global_configuration();
  if (global_config.estimable_value_file() != "") {
    Estimables& estimables = *model->managers().estimables();
    vector<string> estimable_labels = estimables.GetEstimables();

    for (string label : estimable_labels) {
      bool match = [](string label, vector<Estimate*> objects) -> bool {
        for (Estimate* estimate : objects) {
          if (estimate->label() == label)
            return true;
        }
        return false;
      };
      if (!match)
        LOG_ERROR() << "The estimable " << label << " was defined in the estimable value file, but has not been defined as an @estimate";
    }
  }
}

/**
 *
 */
void Manager::Build() {
  LOG_CODE_ERROR() << "This method is not supported";
}

/**
 * Build our estimates
 */
void Manager::Build(Model* model) {
  for (auto estimate : objects_) {
    estimate->Build();
  }

  if (model->global_configuration().create_mpd_file()) {
    model->managers().report()->Pause();

    reports::MPD* report = new reports::MPD(model);
    report->set_block_type(PARAM_REPORT);
    report->set_defined_file_name(__FILE__);
    report->set_defined_line_number(__LINE__);
    report->parameters().Add(PARAM_LABEL, "mpd", __FILE__, __LINE__);
    report->parameters().Add(PARAM_TYPE, PARAM_MPD, __FILE__, __LINE__);
    report->parameters().Add(PARAM_FILE_NAME, "mpd.out", __FILE__, __LINE__);
    report->Validate();
    model->managers().report()->AddObject(report);

    model->managers().report()->Resume();
  }
}


/**
 * Count how many of our estimates are enabled
 * and return the count
 *
 * @return The number of enabled estimates
 */
unsigned Manager::GetIsEstimatedCount() {
  unsigned count = 0;

  for (Estimate* estimate : objects_) {
    if (estimate->estimated())
      count++;
  }

  return count;
}

/**
 *
 */
vector<Estimate*> Manager::GetIsEstimated() {
  vector<Estimate*> result;

  for (Estimate* estimate : objects_) {
    if (estimate->estimated())
      result.push_back(estimate);
  }

  return result;
}

/**
 *
 */
vector<Estimate*> Manager::GetInObjectiveFunction() {
  vector<Estimate*> result;

  for (Estimate* estimate : objects_) {
    if (estimate->in_objective_function())
      result.push_back(estimate);
  }

  return result;
}

/**
 *
 */
void Manager::Clear() {
  objects_.clear();
  creators_.clear();
}

/**
 * This method will look for estimates that match our parameter.
 *
 * First it will check all of the parameters parent info object
 * (the object that created them) for matches. If it doesn't
 * find a match then it'll re-loop through the estimates
 * looking for a specific parameter there.
 *
 * This is because we want to prioritise matches to what was
 * defined explicitly in the configuration file. As each @estimate
 * block in the file can create multiple Estimate objects we
 * will prioritise this way first before looking at what
 * actual estimates were created.
 *
 * @param parameter The parameter the estimate is targeting
 * @return True if found, false otherwise
 */
bool Manager::HasEstimate(const string& parameter) {
  for (Estimate* estimate : objects_) {
    if (estimate->parameter() == parameter)
      return true;
  }

  for (Estimate* estimate : objects_) {
    if (estimate->creator_parameter() == parameter)
      return true;
  }

  return false;
}

/**
 * This method will enable all estimates that match either the parameter
 * or the parent info matches the parameter.
 *
 * @param parameter The parameter to match against the estimate and parent info
 */
void Manager::FlagIsEstimated(const string& parameter) {
  for (Estimate* estimate : objects_) {
    if (estimate->creator_parameter() == parameter)
      estimate->set_estimated(true);
    else if (estimate->parameter() == parameter)
      estimate->set_estimated(true);
  }
}

/**
 * This method will disable all estimates that match either the parameter
 * or the parent info matches the parameter.
 *
 * @param parameter The parameter to match against the estimate and parent info
 */
void Manager::UnFlagIsEstimated(const string& parameter) {
  for (Estimate* estimate : objects_) {
    if (estimate->creator_parameter() == parameter)
      estimate->set_estimated(false);
    else if (estimate->parameter() == parameter)
      estimate->set_estimated(false);
  }
}

/**
 *
 */
Estimate* Manager::GetEstimate(const string& parameter) {
  for (Estimate* estimate : objects_) {
    if (estimate->parameter() == parameter)
      return estimate;
  }
  return nullptr;;
}

/**
 *
 */
Estimate* Manager::GetEstimateByLabel(const string& label) {
  LOG_FINEST() << "Checking for estimate by label " << label;
  Estimate* result = nullptr;

  unsigned count = 0;
  for (Estimate* estimate : objects_) {
    if (estimate->label() == label) {
      result = estimate;
      ++count;
    }
  }
  if (count > 1)
    LOG_ERROR() << "Expected one parameter with the label " << label << " but found " << count;

  return result;
}

/**
 *
 */
vector<Estimate*> Manager::GetEstimatesByLabel(const string& label) {
  LOG_FINEST() << "Checking for estimate by label " << label;
  vector<Estimate*> result;

  unsigned count = 0;
  for (Estimate* estimate : objects_) {
    if (estimate->label() == label) {
      result.push_back(estimate);
      ++count;
    }
  }

  if (count > 1)
    LOG_FINEST() << "There were " << count << " parameters retrieved in the @estimate block labeled " << label;

  return result;
}


} /* namespace estimates */
} /* namespace niwa */
