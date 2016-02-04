/**
 * @file ObjectiveFunction.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 21/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "ObjectiveFunction.h"

#include "AdditionalPriors/Manager.h"
#include "Estimates/Manager.h"
#include "EstimateTransformations/Manager.h"
#include "Model/Model.h"
#include "Observations/Manager.h"
#include "Penalties/Manager.h"
#include "Utilities/To.h"

// Namespaces
namespace niwa {

/**
 * Default constructor
 */
ObjectiveFunction::ObjectiveFunction(Model* model) : model_(model) {
}

/**
 * Clear our objective function so it's values are not carried
 * over accidentally
 */
void ObjectiveFunction::Clear() {
  score_        = 0.0;
  penalties_    = 0.0;
  priors_       = 0.0;
  likelihoods_  = 0.0;
  additional_priors_ = 0.0;
  score_list_.clear();
}

/**
 * Calculate our score for the current run
 */
void ObjectiveFunction::CalculateScore() {
  Clear();

  /**
   * Get the scores from each of the observations/likelihoods
   */
  vector<Observation*> observations = model_->managers().observation()->objects();
  likelihoods_ = 0.0;
  for(auto observation : observations) {
    const map<unsigned, Double>& scores = observation->scores();
    bool append_age = scores.size() > 1 ? true : false;
    for(auto iter = scores.begin(); iter != scores.end(); ++iter) {
      objective::Score new_score;
      new_score.label_ = PARAM_OBS + string("->") + observation->label();
      if (append_age)
        new_score.label_ += string("-") + utilities::ToInline<unsigned, string>(iter->first);
      new_score.score_ = iter->second;

      score_list_.push_back(new_score);
      score_ += new_score.score_;
      likelihoods_ += AS_DOUBLE(new_score.score_);
    }
  }

  /**
   * Get the scores from each of the penalties
   */
  penalties_ = 0.0;

  for (auto penalty : model_->managers().penalty()->objects()) {
    if (penalty->has_score()) {
      objective::Score new_score;

      new_score.label_ = PARAM_PENALTY + string("->") + penalty->label();
      new_score.score_ = penalty->GetScore();

      score_list_.push_back(new_score);
      score_ += new_score.score_;
      penalties_ += AS_DOUBLE(new_score.score_);
    }
  }

  /**
   * Go through the flagged penalties
   */
  const vector<penalties::Info>& penalties = model_->managers().penalty()->flagged_penalties();
  for (penalties::Info penalty : penalties) {
    objective::Score new_score;

    new_score.label_ = PARAM_PENALTY + string("->") + penalty.label_;
    new_score.score_ = penalty.score_;

    score_list_.push_back(new_score);
    score_ += new_score.score_;
    penalties_ += AS_DOUBLE(new_score.score_);
  }

  /**
   * Get the scores from each of the estimate priors
   */
  vector<Estimate*> estimates = model_->managers().estimate()->objects();
  priors_ = 0.0;
  for (Estimate* estimate : estimates) {
    if (!estimate->in_objective_function())
      continue;
    objective::Score new_score;
    if (estimate->label() != "")
      new_score.label_ = PARAM_PRIOR + string("->") + estimate->label() + "->" + estimate->parameter();
    else
      new_score.label_ = PARAM_PRIOR + string("->") + estimate->parameter();
    new_score.score_ = estimate->GetScore();

    score_list_.push_back(new_score);
    score_ += new_score.score_;
    priors_ += AS_DOUBLE(new_score.score_);
  }

  /**
   * Get the score from each additional prior
   */
  vector<AdditionalPrior*> additional_priors = model_->managers().additional_prior()->objects();
  additional_priors_ = 0.0;
  for (auto prior : additional_priors) {
    objective::Score new_score;
    new_score.label_ = PARAM_ADDITIONAL_PRIOR + string("->") + prior->label();
    new_score.score_ = prior->GetScore();

    score_list_.push_back(new_score);
    score_ += new_score.score_;
    additional_priors_ += AS_DOUBLE(new_score.score_);
  }

  auto jacobians = model_->managers().estimate_transformation()->objects();
  jacobians_ = 0.0;
  for (auto jacobian : jacobians) {
    objective::Score new_score;
    new_score.label_ = PARAM_JACOBIAN + string("->") = jacobian->label();
    new_score.score_ = jacobian->GetScore();

    score_list_.push_back(new_score);
    score_ += new_score.score_;
    jacobians_ += AS_DOUBLE(new_score.score_);
  }
}

} /* namespace niwa */
