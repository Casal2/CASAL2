/**
 * @file UnsexedLengthBased.h
 * @author C.Marsh
 * @github https://github.com/Craig44
 * @date 2020
 * @section LICENSE
 *
 * Copyright NIWA Science 2020 - www.niwa.co.nz
 */
#ifndef TESTCASES_MODELS_LENGTHBASED_UNSEXED_
#define TESTCASES_MODELS_LENGTHBASED_UNSEXED_

#ifdef TESTMODE

#include <string>

namespace niwa {
namespace testresources {
namespace models {

/**
 *
 */
const std::string length_based_unsexed_basic =
R"(
    @model
    type length
    start_year 1986 
    final_year 2012
    projection_final_year 2021
    length_bins  1:68
    length_plus false
    length_plus_group 69
    base_weight_units tonnes
    initialisation_phases Equilibrium_state
    time_steps Annual

    @categories 
    format sex
    names uni
    growth_increment growth_model


    @initialisation_phase Equilibrium_state
    type iterative
    years 200
    convergence_years 200

    @time_step Annual 
    processes Recruit_BH growth  mortality

    @process Nop
    type null_process

    @process Recruit_BH
    type recruitment_beverton_holt
    ssb_offset 1
    ycs_years 1985:2011
    standardise_ycs_years 1985:2009
    ycs_values 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00  1.00 1.00 1.00  1.00 1.00 1.00
    b0 1500
    categories uni
    proportions 1
    steepness 0.75
    inital_mean_length 10
    inital_length_cv 0.40
    ssb SSB

    @process mortality
    type mortality_constant_rate
    m 0.2
    relative_m_by_length One
    time_step_proportions 1
    categories uni

    @process growth
    type growth
    categories uni

    @growth_increment growth_model ##
    type basic
    time_step_proportions 1
    l_alpha 20
    l_beta  40
    g_alpha 10
    g_beta 1
    min_sigma 2
    distribution normal
    length_weight allometric
    cv 0.0
    compatibility_option casal

    @length_weight allometric
    type basic
    a 0.000000000373
    b 3.145
    units tonnes

    @derived_quantity SSB
    type biomass
    categories uni
    selectivities maturity
    time_step Annual
    time_step_proportion 0.5

    @selectivity double_normal
    type double_normal
    mu 21
    sigma_l 5
    sigma_r 10

    @selectivity maturity
    type logistic
    a50 30 
    ato95 5

    @selectivity One
    type constant
    c 1
)";

}  // namespace models
}  // namespace testresources
}  // namespace niwa

#endif /* TESTMODE */
#endif /* TESTCASES_MODELS_LENGTHBASED_UNSEXED_ */