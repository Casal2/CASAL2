/**
 * @file Data.Test.cpp
 * @author C.Marsh
 * @github https://github.com
 * @date 4/01/2018
 * @section LICENSE
 *
 * Copyright NIWA Science ©2018 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include <iostream>

#include "Age/AgeWeights/Manager.h"
#include "Common/ObjectiveFunction/ObjectiveFunction.h"
#include "Common/Estimates/Manager.h"
#include "Common/Model/Model.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TestResources/Models/TwoSexHalfAges.h"

// Namespaces
namespace niwa {
namespace estimates {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;

const string age_weight =
R"(
@model
start_year 1986 
final_year 2005
projection_final_year 2006
min_age 1
max_age 10
base_weight_units tonnes
plus_group true
initialisation_phases Equilibrium_phase Non_equilibrium_phase
time_steps Annual Ageing

@categories 
format stock
names Stock

@initialisation_phase Equilibrium_phase
type derived

@initialisation_phase Non_equilibrium_phase
type cinitial
categories Stock
table n
Stock 10000 9000  8000  7000  6000  5000  4000  3000  2000  2000 
end_table

@time_step Annual 
processes Recruitment Instantaneous_Mortality

@time_step Ageing
processes Ageing

@process Recruitment
type recruitment_beverton_holt_with_deviations
categories Stock
proportions 1.0
b0 5973.9         
last_year_with_no_bias 1986
first_year_with_bias 1986
last_year_with_bias 2005 
first_recent_year_with_no_bias 2005
b_max 1.0
deviation_values 0.161055491247 0.0935823574532 -0.454148603651 -0.143755692424 -0.979662527968 1.70885246646 0.207878584523 -0.142998000652 0.448480496158 -1.25142717617 0.621050539438 0.0732265269830 0.0247523483886 0.137793954591 0.199439856167 -0.701101119095 0.613584736138 -0.103102553946 -0.101487357908 -0.412014325737
steepness 0.5792984
ssb SSB
age 1     
sigma_r 0.6
deviation_years 1985:2004

@process Ageing
type ageing
categories *

@process Instantaneous_Mortality
type mortality_instantaneous
m 0.3
time_step_ratio 1.0
selectivities One
categories Stock
table catches
year Fishery
1986  82.2
1987  413
1988  543.6
1989  545.4
1990  344.5
1991  469.2
1992  1019.4
1993  705.2
1994  925.4
1995  894.7
1996  893.3
1997  645.5
1998  431.5
1999  95.2
2000  146.4
2001  103
2002  99
2003  45.6
2004  157.5
2005  119
end_table

table method
method    category    selectivity   u_max   time_step   penalty   age_weight_label
Fishery     Stock       Fishing   0.99  Annual    none    weight_matrix_1
end_table

@derived_quantity SSB
type biomass
time_step Annual
categories Stock
time_step_proportion 0.35
time_step_proportion_method weighted_sum
selectivities MaturationSel
age_weight_labels weight_matrix_2

@selectivity MaturationSel
type all_values
v 0.0180     0.1192     0.5000     0.8808     0.9820     0.9975     0.9997     1.0000     1.0000     1.0000 

@selectivity Logistic
type logistic
a50 3.26084601863
ato95 0.638729032931
alpha 1.0

@selectivity Fishing
type all_values
v 0.154297 0.349839 0.614098 0.879678 0.945269 0.926326 1 1 1 1

@selectivity One
type constant 
c 1

@selectivity AgeOne
type all_values
v 1 0 0 0 0 0 0 0 0 0

@age_weight weight_matrix_1
type Data
units tonnes
table data
year 1 2 3 4 5 6 7 8 9 10
1986  0.134 0.686 1.639 2.719 3.649 4.901 6.329 6.591 7.238 7.491
1987  0.132 0.724 1.534 2.829 4.092 4.853 5.705 6.143 7.179 8.089
1988  0.122 0.641 1.533 2.641 3.796 5.054 5.652 6.356 6.95  8.857
1989  0.137 0.722 1.606 2.416 3.629 5.027 5.561 6.35  6.933 7.217
1990  0.138 0.773 1.645 2.74  3.711 4.506 5.684 6.929 7.424 7.479
1991  0.135 0.689 1.683 2.571 3.514 4.777 5.416 6.614 6.998 7.387
1992  0.148 0.691 1.521 2.506 3.474 4.734 5.422 6.158 7.245 8.554
1993  0.149 0.753 1.541 2.615 3.697 4.425 5.807 6.27  7.403 8.172
1994  0.147 0.725 1.606 2.623 3.73  4.793 5.9 6.136 7.203 7.853
1995  0.157 0.662 1.533 2.803 3.737 4.885 6.338 6.363 8.116 7.306
1996  0.142 0.733 1.595 2.74  3.498 4.916 5.604 6.057 7.077 8.432
1997  0.135 0.707 1.428 2.625 3.639 4.473 5.901 6.982 7.878 7.53
1998  0.139 0.733 1.527 2.601 3.702 4.604 6.051 6.455 6.712 7.425
1999  0.13  0.72  1.601 2.884 3.516 4.864 5.531 6.46  7.331 7.432
2000  0.143 0.665 1.514 2.706 3.816 4.772 5.364 6.256 7.256 7.534
2001  0.142 0.654 1.54  2.757 4.214 5.232 5.652 6.758 6.734 8.103
2002  0.15  0.665 1.446 2.751 3.684 4.494 5.346 6.645 7.144 7.219
2003  0.138 0.672 1.525 2.549 3.266 4.618 5.817 6.341 7.666 8.191
2004  0.141 0.686 1.391 2.876 3.504 4.855 5.6 6.252 7.764 7.615
2005  0.141 0.708 1.474 2.881 3.693 4.844 5.593 6.146 7.654 7.759
end_table



@age_weight weight_matrix_2
type Data
units tonnes
table data
year 1 2 3 4 5 6 7 8 9 10
1986  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
1987  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
1988  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
1989  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
1990  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
1991  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
1992  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
1993  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
1994  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
1995  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
1996  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
1997  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
1998  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
1999  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
2000  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
2001  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
2002  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
2003  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
2004  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
2005  0.14  0.7 1.544 2.692 3.678 4.781 5.714 6.413 7.295 7.782
end_table

@catchability Index1Q
type free
q 0.12890990


@catchability Index2Q
type free
q 0.02489229


@observation Index1
type biomass 
time_step Annual
time_step_proportion 0.5
categories Stock
selectivities Logistic
catchability Index1Q
years 1986  1987  1988  1989  1990  1991  1992  1993  1994  1995  1996  1997  1998  1999  2000  2001  2002  2003  2004  2005
obs 728 522 804 454 456 530 218 367 366 176 180 154 109 145 140 160 142 215 156 198
error_value 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3
likelihood lognormal

@observation Index2
type abundance 
time_step Annual
time_step_proportion 0.25
categories Stock
selectivities AgeOne
catchability Index2Q
years   1986  1987  1988  1989  1990  1991  1992  1993  1994  1995  1996  1997  1998  1999  2000  2001  2002  2003  2004  2005
obs   5 13.1  14.3  13  3.3 15  10.8  12.1  8.6 6.8 13  7.3 7.4 24.3  4.4 3.3 12.3  12.1  16.8  25.2
error_value 0.65  0.65  0.65  0.65  0.65  0.65  0.65  0.65  0.65  0.65  0.65  0.65  0.65  0.65  0.65  0.65  0.65  0.65  0.65  0.65
likelihood lognormal

@observation Index1_age
type proportions_at_age
likelihood multinomial
age_plus true
ageing_error Ageing_error
delta 1e-011
time_step Annual
min_age 1
max_age 10
categories Stock
selectivities Logistic
years 1997:2005
table obs
1997  0.015 0.151 0.052 0.261 0.108 0.095 0.273 0.01  0.014 0.021
1998  0.017 0.109 0.362 0.065 0.197 0.069 0.056 0.108 0.004 0.013
1999  0.022 0.11  0.221 0.383 0.039 0.111 0.036 0.023 0.048 0.007
2000  0.022 0.107 0.196 0.229 0.282 0.027 0.075 0.02  0.013 0.029
2001  0.01  0.111 0.196 0.209 0.172 0.206 0.019 0.042 0.012 0.023
2002  0.031 0.046 0.205 0.213 0.166 0.132 0.15  0.011 0.026 0.02
2003  0.018 0.16  0.084 0.222 0.17  0.128 0.097 0.088 0.007 0.026
2004  0.013 0.085 0.285 0.089 0.175 0.13  0.093 0.057 0.054 0.019
2005  0.015 0.076 0.156 0.313 0.071 0.137 0.097 0.056 0.036 0.043
end_table

table error_values
1997  200
1998  200
1999  200
2000  200
2001  200
2002  200
2003  200
2004  200
2005  200
end_table

@observation catch_at_age
type process_removals_by_age
likelihood multinomial
time_step Annual
mortality_instantaneous_process Instantaneous_Mortality
categories Stock
method_of_removal Fishery
years 1986:2005
min_age 1
max_age 10
ageing_error Ageing_error
age_plus true
table obs
1986  0.04  0.165 0.095 0.23  0.16  0.1 0.03  0.095 0.03  0.055
1987  0.02  0.09  0.16  0.1 0.185 0.18  0.08  0.01  0.09  0.085
1988  0.05  0.105 0.145 0.18  0.09  0.13  0.1 0.085 0.005 0.11
1989  0.04  0.08  0.13  0.135 0.17  0.07  0.095 0.09  0.085 0.105
1990  0.02  0.1 0.065 0.185 0.14  0.17  0.05  0.075 0.06  0.135
1991  0.37  0.025 0.095 0.065 0.115 0.07  0.05  0.035 0.035 0.14
1992  0.065 0.555 0.045 0.095 0.045 0.045 0.06  0.015 0.02  0.055
1993  0.055 0.12  0.53  0.03  0.09  0.025 0.025 0.015 0.04  0.07
1994  0.095 0.08  0.125 0.5 0.025 0.02  0.02  0.035 0.02  0.08
1995  0.025 0.225 0.105 0.155 0.36  0.005 0.04  0.01  0.01  0.065
1996  0.2 0.025 0.26  0.08  0.11  0.235 0.02  0.005 0.01  0.055
1997  0.11  0.38  0.055 0.185 0.06  0.02  0.165 0.015 0.005 0.005
1998  0.135 0.175 0.445 0.02  0.095 0.03  0.025 0.07  0 0.005
1999  0.095 0.17  0.255 0.285 0.03  0.09  0.02  0 0.03  0.025
2000  0.105 0.155 0.2 0.25  0.225 0.01  0.025 0.01  0.005 0.015
2001  0.045 0.175 0.195 0.21  0.15  0.175 0.005 0.02  0.005 0.02
2002  0.22  0.045 0.215 0.21  0.115 0.1 0.07  0 0.015 0.01
2003  0.095 0.235 0.08  0.22  0.125 0.08  0.1 0.05  0 0.015
2004  0.06  0.085 0.37  0.11  0.185 0.065 0.04  0.03  0.03  0.025
2005  0.03  0.125 0.125 0.365 0.075 0.1 0.085 0.045 0.025 0.025
end_table

table error_values
1986  150
1987  150
1988  150
1989  150
1990  150
1991  150
1992  150
1993  150
1994  150
1995  150
1996  150
1997  150
1998  150
1999  150
2000  150
2001  150
2002  150
2003  150
2004  150
2005  150
end_table


@ageing_error Ageing_error
type none


@report obj
type objective_function 
)";

/**
 *
 */
TEST_F(InternalEmptyModel, AgeWeight_Data) {
  AddConfigurationLine(age_weight, __FILE__, 33);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(3790.6915394385342, obj_function.score());
}

} /* namespace estimates */
} /* namespace niwa */
#endif
