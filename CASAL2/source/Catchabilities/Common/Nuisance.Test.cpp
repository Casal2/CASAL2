/**
 * @file Nuisance.Test.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 18/08/2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include <iostream>

#include "../../Catchabilities/Manager.h"
#include "../../Model/Factory.h"
#include "../../Model/Models/Age.h"
#include "../../Partition/Partition.h"
#include "../../TestResources/TestFixtures/InternalEmptyModel.h"
#include "../../TimeSteps/Manager.h"
#include "Nuisance.h"

// Namespaces
namespace niwa {
namespace catchabilities {

using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;

/**
 *
 */
const std::string base_model =
    R"(
@model
start_year 1975 
final_year 2012
min_age 1
max_age 30
age_plus true
base_weight_units tonnes
initialisation_phases iphase1
time_steps step1 step2 step3

@categories 
format stock 
names stock
age_lengths age_size 

@initialisation_phase iphase1
type derived

@time_step step1 
processes Recruitment instant_mort

@time_step step2 
processes instant_mort 

@time_step step3 
processes  Ageing instant_mort

@process Recruitment
type recruitment_beverton_holt
categories stock
proportions 1 
b0 52583
standardise_ycs_years 1975 1976 1977 1978 1979 1980 1981 1982 1983 1984 1985 1986 1987 1988 1989 1990 1991 1992 1993 1994 1995 1996 1997 1998 1999 2000 2001 2002 2003 2004 2005 2006 2007 2008 2009
ycs_values          1.00 1.02472  1.04658 1.06324 1.07299 1.07458 1.06541 1.04328 1.01815 1.00285 0.426938  0.476756  0.520428  0.382199  0.527158  0.445187  0.67427 1.00397 0.761247  0.653433  0.592745  0.488619  0.321246  0.303421  0.221372  0.33299 0.219917  0.328752  0.925172  0.525766  0.434871  0.18722 0.307342  0.259689  0.230941  0.260211  3 3
steepness 0.9
ssb biomass_t1
age 1
ycs_years 1974:2011

@process Ageing
type ageing
categories stock


@process instant_mort
type mortality_instantaneous
m 0.19
time_step_ratio 0.42 0.25 0.33
relative_m_by_age One
categories stock
table catches
year FishingWest FishingEest
1975  80  111
1976  152 336
1977  74  1214
1978  28  6
1979  103 506
1980  481 269
1981  914 83
1982  393 203
1983  154 148
1984  224 120
1985  232 312
1986  282 80
1987  387 122
1988  385 189
1989  386 418
1990  309 689
1991  409 503
1992  718 1087
1993  656 1996
1994  368 2912
1995  597 2903
1996  1353  2483
1997  1475  1820
1998  1424  1124
1999  1169  3339
2000  1155  2130
2001  1208  1700
2002  454 1058
2003  497 718
2004  687 1983
2005  2585  1434
2006  184 255
2007  270 683
2008  259 901
2009  1069  832
2010  231 159
2011  822 118
2012  800 150
end_table

table method
method    category  selectivity   u_max   time_step   penalty
FishingWest     stock     westFSel  0.7   step1     none
FishingEest   stock     eastFSel  0.7   step1     none
end_table

@derived_quantity biomass_t1
type biomass
time_step step1
categories stock
time_step_proportion 0.5
time_step_proportion_method weighted_sum
selectivities MaturationSel

@selectivity MaturationSel
type all_values_bounded
l 2
h 13
v 0.02  0.05  0.13  0.29  0.50  0.70  0.84  0.93  0.97  0.99  0.99  1.00

@selectivity westFSel
type double_normal
mu 6.44798
sigma_l  3.00327
sigma_r 7.88331
alpha 1.0

@selectivity eastFSel
type double_normal
mu 10.8129
sigma_l  3.50265
sigma_r 6.86988
alpha 1.0

@selectivity chatTANSel
type double_normal
mu 9.51995
sigma_l 5.53388
sigma_r 10.8664
alpha 1.0

@selectivity One
type constant
c 1

@age_length age_size
type schnute
by_length true
time_step_proportions 0.25 0.5 0.0
y1 24.5
y2 104.8
tau1 1
tau2 20
a 0.131
b 1.70
cv_first 0.1
length_weight size_weight3


@length_weight size_weight3
type basic
units kgs
a 2.0e-6
b 3.288

@observation chatTANage
type proportions_at_age
likelihood multinomial
time_step step1
categories stock
min_age 3
max_age 19
plus_group true
selectivities chatTANSel
years 1990 1992:2012
table obs
1990    0.1235  0.1068  0.0935  0.0939  0.1065  0.0845  0.0544  0.1283  0.0489  0.0430  0.0162  0.0401  0.0162  0.0274  0.0035  0.0025  0.0107
1992    0.0240  0.0471  0.0446  0.0708  0.0778  0.1039  0.0671  0.1212  0.0870  0.1111  0.0791  0.0438  0.0412  0.0244  0.0290  0.0121  0.0158
1993    0.0999  0.0576  0.0828  0.0412  0.0582  0.0646  0.0401  0.0571  0.0587  0.0842  0.1388  0.0410  0.0636  0.0432  0.0320  0.0111  0.0260
1994    0.2247  0.0891  0.0852  0.0787  0.0450  0.0532  0.0265  0.0352  0.0376  0.0507  0.0209  0.0497  0.0251  0.0312  0.0476  0.0335  0.0663
1995    0.1730  0.1344  0.0868  0.0491  0.0759  0.0608  0.0229  0.0623  0.0375  0.0543  0.0517  0.0930  0.0294  0.0168  0.0222  0.0201  0.0100
1996    0.1225  0.1991  0.2293  0.1278  0.0678  0.0580  0.0198  0.0431  0.0038  0.0206  0.0328  0.0242  0.0136  0.0151  0.0145  0.0079  0.0001
1997    0.2073  0.1483  0.1522  0.1318  0.0544  0.0760  0.0225  0.0259  0.0091  0.0179  0.0392  0.0350  0.0154  0.0019  0.0318  0.0062  0.0250
1998    0.0596  0.1291  0.1527  0.1293  0.1389  0.0880  0.0489  0.0393  0.0451  0.0398  0.0160  0.0307  0.0117  0.0123  0.0147  0.0192  0.0247
1999    0.1308  0.1072  0.1112  0.0771  0.0600  0.1120  0.0669  0.0692  0.0370  0.0382  0.0227  0.0204  0.0032  0.0305  0.0191  0.0279  0.0668
2000    0.0751  0.0821  0.0855  0.0724  0.0813  0.1222  0.0921  0.0678  0.0684  0.0507  0.0308  0.0599  0.0231  0.0198  0.0227  0.0031  0.0429
2001    0.0097  0.0587  0.0832  0.0623  0.1438  0.1161  0.1212  0.1086  0.1065  0.0692  0.0200  0.0245  0.0056  0.0138  0.0226  0.0047  0.0296
2002    0.0527  0.0445  0.0387  0.0478  0.0913  0.1144  0.1172  0.1324  0.1077  0.0952  0.0576  0.0287  0.0131  0.0098  0.0027  0.0047  0.0413
2003    0.0388  0.1001  0.0304  0.1096  0.0609  0.1157  0.0837  0.0741  0.1217  0.0897  0.0349  0.0433  0.0176  0.0002  0.0076  0.0133  0.0586
2004    0.0319  0.0301  0.0306  0.0536  0.0893  0.0821  0.1564  0.0579  0.1151  0.0998  0.1081  0.0475  0.0339  0.0179  0.0131  0.0002  0.0325
2005    0.4424  0.1235  0.0440  0.0430  0.0604  0.0319  0.0147  0.0492  0.0292  0.0148  0.0282  0.0292  0.0154  0.0135  0.0077  0.0136  0.0392
2006    0.2396  0.2879  0.0870  0.0462  0.0703  0.0670  0.0324  0.0317  0.0368  0.0283  0.0237  0.0115  0.0121  0.0133  0.0100  0.0001  0.0022
2007    0.0993  0.1484  0.2786  0.1096  0.0726  0.0281  0.0516  0.0376  0.0495  0.0135  0.0165  0.0319  0.0388  0.0064  0.0106  0.0001  0.0068
2008    0.0153  0.1008  0.1202  0.3307  0.0911  0.1118  0.0304  0.0941  0.0185  0.0213  0.0187  0.0138  0.0172  0.0001  0.0097  0.0061  0.0001
2009    0.0539  0.0498  0.1413  0.1398  0.2082  0.1102  0.0759  0.0550  0.0394  0.0249  0.0247  0.0270  0.0184  0.0172  0.0102  0.0024  0.0016
2010    0.0722  0.0890  0.1032  0.1441  0.1502  0.1771  0.0720  0.0446  0.0387  0.0193  0.0163  0.0221  0.0219  0.0173  0.0058  0.0061  0.0001
2011    0.0630  0.0511  0.1770  0.0599  0.1653  0.1108  0.1187  0.0711  0.0168  0.0340  0.0430  0.0095  0.0011  0.0211  0.0241  0.0319  0.0019
2012    0.0491  0.0619  0.1059  0.1332  0.0625  0.1253  0.1391  0.0591  0.0947  0.0313  0.0382  0.0314  0.0111  0.0103  0.0001  0.0074  0.0394
end_table

table error_values
1990 25
1992 37
1993 31
1994 34
1995 22
1996 20
1997 21
1998 19
1999 17
2000 31
2001 23
2002 24
2003 10
2004 16
2005 16
2006 20
2007 21
2008 17
2009 27
2010 16
2011 13
2012 13
end_table
time_step_proportion 1.0


@observation chatOBSwst
type process_removals_by_age
likelihood multinomial
time_step step1
mortality_instantaneous_process instant_mort
categories stock
method_of_removal FishingWest
ageing_error Ageing_error
min_age 3
max_age 19
plus_group true
years 1992 1994 1995 1996 1997 1998 1999 2000 2001 2002 2003 2004 2005 2006 2008 2009 2010 2011

table obs
1992    0.0583  0.0978  0.0796  0.1452  0.0885  0.1002  0.0517  0.0995  0.0585  0.0607  0.0560  0.0233  0.0296  0.0140  0.0167  0.0080  0.0122
1994    0.1820  0.1107  0.1768  0.1345  0.1009  0.0905  0.0448  0.0175  0.0175  0.0153  0.0009  0.0209  0.0154  0.0146  0.0130  0.0176  0.0271
1995    0.3379  0.1712  0.1471  0.1346  0.0573  0.0429  0.0037  0.0367  0.0092  0.0125  0.0079  0.0175  0.0054  0.0030  0.0028  0.0100  0.0002
1996    0.1625  0.2141  0.2736  0.1439  0.0654  0.0669  0.0143  0.0261  0.0041  0.0046  0.0054  0.0059  0.0105  0.0003  0.0017  0.0004  0.0001
1997    0.1477  0.2041  0.2076  0.1430  0.0652  0.1273  0.0146  0.0103  0.0272  0.0038  0.0167  0.0085  0.0034  0.0007  0.0049  0.0028  0.0121
1998    0.2036  0.1767  0.1548  0.1090  0.1254  0.0962  0.0381  0.0360  0.0185  0.0115  0.0063  0.0073  0.0045  0.0038  0.0034  0.0007  0.0041
1999    0.0756  0.1427  0.1226  0.1509  0.0957  0.1168  0.0806  0.0671  0.0482  0.0246  0.0138  0.0188  0.0048  0.0116  0.0064  0.0041  0.0159
2000    0.0457  0.0861  0.0885  0.0925  0.1439  0.1583  0.1224  0.0803  0.0704  0.0353  0.0176  0.0159  0.0125  0.0095  0.0083  0.0020  0.0107
2001    0.0140  0.0839  0.1140  0.1084  0.2003  0.1574  0.0950  0.0726  0.0573  0.0322  0.0146  0.0103  0.0115  0.0064  0.0056  0.0063  0.0102
2002    0.0641  0.1068  0.0903  0.0942  0.1319  0.1300  0.1041  0.0972  0.0659  0.0555  0.0170  0.0186  0.0121  0.0015  0.0016  0.0003  0.0090
2003    0.0615  0.2039  0.1448  0.1786  0.1252  0.1017  0.0479  0.0395  0.0182  0.0174  0.0164  0.0131  0.0224  0.0001  0.0024  0.0036  0.0032
2004    0.0315  0.0391  0.0515  0.0571  0.1597  0.1345  0.1522  0.1204  0.0725  0.0565  0.0463  0.0284  0.0128  0.0100  0.0103  0.0002  0.0171
2005    0.0214  0.0754  0.0595  0.2281  0.2038  0.1148  0.0857  0.0869  0.0503  0.0301  0.0134  0.0149  0.0001  0.0007  0.0001  0.0128  0.0019
2006    0.1661  0.2186  0.1687  0.0753  0.1089  0.1103  0.0325  0.0358  0.0158  0.0214  0.0326  0.0042  0.0030  0.0024  0.0023  0.0005  0.0016
2008    0.0262  0.1633  0.2390  0.2250  0.1453  0.0861  0.0460  0.0186  0.0125  0.0091  0.0085  0.0083  0.0049  0.0026  0.0008  0.0018  0.0020
2009    0.0093  0.0663  0.1699  0.2166  0.2760  0.0985  0.0722  0.0289  0.0225  0.0113  0.0057  0.0096  0.0001  0.0065  0.0027  0.0001  0.0038
2010    0.0458  0.1024  0.1382  0.1379  0.1898  0.1810  0.0890  0.0316  0.0220  0.0080  0.0192  0.0087  0.0096  0.0084  0.0021  0.0037  0.0026
2011    0.0559  0.1454  0.1752  0.1189  0.1416  0.0917  0.1008  0.0425  0.0139  0.0200  0.0335  0.0033  0.0177  0.0189  0.0062  0.0068  0.0078
end_table

table error_values
1992 27
1994  9
1995 11
1996 11
1997  7
1998 55
1999 30
2000 31
2001 32
2002 23
2003 14
2004 15
2005 17
2006  8
2008 19
2009 12
2010 12
2011  9
end_table

@ageing_error Ageing_error
type none

@observation chatOBSest
type process_removals_by_age
likelihood multinomial
time_step step1
mortality_instantaneous_process instant_mort
categories stock
ageing_error Ageing_error
method_of_removal FishingEest
min_age 4
max_age 21
plus_group true
years 1992 1995 1997 1998 2001 2004 2007

table obs
1992    0.0280  0.0241  0.0979  0.0570  0.1119  0.0708  0.1301  0.0906  0.1035  0.0960  0.0431  0.0715  0.0245  0.0334  0.0095  0.0076  0.0003  0.0003
1995    0.0973  0.0744  0.0468  0.1406  0.0826  0.0199  0.0596  0.0579  0.1469  0.0672  0.1135  0.0335  0.0100  0.0180  0.0252  0.0011  0.0054  0.0001
1997    0.1290  0.1608  0.1319  0.0750  0.1510  0.0469  0.0342  0.0383  0.0366  0.0481  0.0563  0.0036  0.0048  0.0207  0.0001  0.0442  0.0005  0.0180
1998    0.0346  0.0816  0.0895  0.1034  0.1289  0.0780  0.0824  0.0659  0.0782  0.0415  0.0505  0.0163  0.0315  0.0360  0.0224  0.0136  0.0291  0.0168
2001    0.0212  0.0348  0.0389  0.1210  0.1385  0.1401  0.1279  0.1272  0.0762  0.0457  0.0308  0.0298  0.0165  0.0159  0.0099  0.0088  0.0035  0.0133
2004    0.0065  0.0115  0.0156  0.0772  0.0858  0.1480  0.1419  0.1247  0.1171  0.1057  0.0379  0.0322  0.0337  0.0157  0.0085  0.0098  0.0055  0.0228
2007    0.0277  0.0732  0.1025  0.1159  0.0942  0.1308  0.0977  0.1001  0.0701  0.0463  0.0455  0.0439  0.0103  0.0246  0.0119  0.0030  0.0001  0.0021
end_table

table error_values
1992 19
1995 18
1997 15
1998 23
2001 53
2004 43
2007 42
end_table

@minimiser gammadiff
type numerical_differences
tolerance 0.00002
iterations 500
evaluations 1000

@mcmc mcmc
start 0
length 4000000
keep 2500
step_size 0.02
adapt_stepsize_at 10000 50000 150000 250000 450000 750000 1400000
proposal_distribution t

@estimate B0
type uniform_log
parameter process[Recruitment].b0
lower_bound 10000
upper_bound 250000

@catchability chatTANq
type nuisance
lower_bound 0.01
upper_bound 0.40

@report DQ
type derived_quantity

)";

const std::string lognormal_biomass =
    R"(
@observation chatTANbiomass
type biomass
time_step step1
categories stock
catchability chatTANq
selectivities chatTANSel
likelihood lognormal
time_step_proportion 1.0
process_error 0.20
years 1992:2012
table obs
1992    4180    0.15
1993    2950    0.17
1994    3353    0.1
1995    3303    0.23
1996    2457    0.13
1997    2811    0.17
1998    2873    0.18
1999    2302    0.12
2000    2091    0.09
2001    1589    0.13
2002    1567    0.15
2003     888    0.16
2004    1547    0.17
2005    1048    0.18
2006    1384    0.19
2007    1824    0.12
2008    1257    0.13
2009    2419    0.21
2010    1700    0.25
2011    1099    0.15
2012    1292    0.15
end_table
)";

const std::string normal_biomass =
    R"(
@observation chatTANbiomass
type biomass
time_step step1
categories stock
catchability chatTANq
selectivities chatTANSel
likelihood normal
time_step_proportion 1.0
process_error 0.20
years 1992:2012
table obs
1992    4180    0.15
1993    2950    0.17
1994    3353    0.1
1995    3303    0.23
1996    2457    0.13
1997    2811    0.17
1998    2873    0.18
1999    2302    0.12
2000    2091    0.09
2001    1589    0.13
2002    1567    0.15
2003     888    0.16
2004    1547    0.17
2005    1048    0.18
2006    1384    0.19
2007    1824    0.12
2008    1257    0.13
2009    2419    0.21
2010    1700    0.25
2011    1099    0.15
2012    1292    0.15
end_table
)";

const std::string lognormal_q =
    R"(
@additional_prior chatTANq
type lognormal
parameter catchability[chatTANq].q
mu 0.16
cv 0.79
)";

const std::string uniform_log_q =
    R"(
@additional_prior chatTANq
type uniform_log
parameter catchability[chatTANq].q
)";

TEST_F(InternalEmptyModel, Catchabilities_nuisance_normal_prior_none) {
  AddConfigurationLine(base_model, __FILE__, 31);
  AddConfigurationLine(normal_biomass, __FILE__, 31);
  LoadConfiguration();
  model_->Start(RunMode::kBasic);
  Catchability* catchability = model_->managers()->catchability()->GetCatchability("chatTANq");
  EXPECT_NEAR(0.05629393317159407, catchability->q(), 1e-6);
}

TEST_F(InternalEmptyModel, Catchabilities_nuisance_normal_prior_none_estimation) {
  AddConfigurationLine(base_model, __FILE__, 31);
  AddConfigurationLine(normal_biomass, __FILE__, 31);
  LoadConfiguration();
  model_->Start(RunMode::kEstimation);
  Catchability* catchability = model_->managers()->catchability()->GetCatchability("chatTANq");
  EXPECT_NEAR(0.052417942189074938, catchability->q(), 1e-6);
}

TEST_F(InternalEmptyModel, Catchabilities_nuisance_normal_prior_uniform_log) {
  AddConfigurationLine(base_model, __FILE__, 31);
  AddConfigurationLine(normal_biomass, __FILE__, 31);
  AddConfigurationLine(uniform_log_q, __FILE__, 31);
  LoadConfiguration();
  model_->Start(RunMode::kBasic);
  Catchability* catchability = model_->managers()->catchability()->GetCatchability("chatTANq");
  EXPECT_NEAR(0.056143632833131847, catchability->q(), 1e-6);
}

TEST_F(InternalEmptyModel, Catchabilities_nuisance_lognormal_prior_uniform_log) {
  AddConfigurationLine(base_model, __FILE__, 31);
  AddConfigurationLine(lognormal_biomass, __FILE__, 31);
  AddConfigurationLine(uniform_log_q, __FILE__, 31);
  LoadConfiguration();
  model_->Start(RunMode::kBasic);
  Catchability* catchability = model_->managers()->catchability()->GetCatchability("chatTANq");
  EXPECT_NEAR(0.057232970885068367, catchability->q(), 1e-6);
}

TEST_F(InternalEmptyModel, Catchabilities_nuisance_lognormal_prior_none) {
  AddConfigurationLine(base_model, __FILE__, 31);
  AddConfigurationLine(lognormal_biomass, __FILE__, 31);
  LoadConfiguration();
  model_->Start(RunMode::kBasic);
  Catchability* catchability = model_->managers()->catchability()->GetCatchability("chatTANq");
  EXPECT_NEAR(0.057403242558276682, catchability->q(), 1e-6);
}
TEST_F(InternalEmptyModel, Catchabilities_nuisance_lognormal_prior_lognormal) {
  AddConfigurationLine(base_model, __FILE__, 31);
  AddConfigurationLine(lognormal_biomass, __FILE__, 31);
  AddConfigurationLine(lognormal_q, __FILE__, 31);
  LoadConfiguration();
  model_->Start(RunMode::kBasic);
  Catchability* catchability = model_->managers()->catchability()->GetCatchability("chatTANq");
  EXPECT_NEAR(0.057507359866387671, catchability->q(), 1e-6);
}
} /* namespace catchabilities */
} /* namespace niwa */

#endif /* TESTMODE */
