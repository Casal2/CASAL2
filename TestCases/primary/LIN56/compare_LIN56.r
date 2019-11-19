## Comparison of CASAL and Casal2 models for LIN5/6
# R 3.6.0 on Linux
#

require(here)

require(casal)
require(casal2)

require(ggplot2)


sessionInfo()


# start in subdirectory 'R-libraries' of git repository
base_dir <- '../TestCases/primary/LIN56'

C2_base_dir <- 'casal_flags_on'
C2_alt1_dir <- 'casal_flags_off'
# C2_alt2_dir <- 'adolc'
C2_alt2_dir <- 'casal_flags_on_low_tol'
C2_alt3_dir <- 'cppad'


# MPD files are params_est.out, run_estimation.txt, and mpd.out for both CASAL and Casal2
params_est <- 'params_est.out'
mpd_run    <- 'run_estimation.txt'
mpd_out    <- 'mpd.out'


# read in CASAL MPD results
cas_mpd  <- casal::extract.mpd(mpd_run, file.path(base_dir, 'CASAL'))
c1_quant <- cas_mpd$quantities

cas_mpd_sens1  <- casal::extract.mpd(mpd_run, file.path(base_dir, 'CASAL_sens1'))
c1_sens1_quant <- cas_mpd_sens1$quantities


# > names(cas_mpd)
# [1] "header"               "objective.function"   "parameters.at.bounds"
# [4] "fits"                 "free"                 "bounds"
# [7] "quantities"
# > names(cas_mpd$quantities)
#  [1] "Scalar parameter values" "Vector parameter values" "Ogive parameter values"
#  [4] "B0"                      "R0"                      "SSBs"
#  [7] "true_YCS"                "actual_catches"          "fishing_pressures"
# [10] "Tangaroa_bio_autumn_est" "Tangaroa_bio_summer_est"


# read in Casal2 MPD results
cas2_mpd_base <- casal2::extract.mpd(mpd_run, file.path(base_dir, 'Casal2', C2_base_dir))
cas2_mpd_alt1 <- casal2::extract.mpd(mpd_run, file.path(base_dir, 'Casal2', C2_alt1_dir))
cas2_mpd_alt2 <- casal2::extract.mpd(mpd_run, file.path(base_dir, 'Casal2', C2_alt2_dir))
cas2_mpd_alt3 <- casal2::extract.mpd(mpd_run, file.path(base_dir, 'Casal2', C2_alt3_dir))

# > names(cas2_mpd_base)
#  [1] "Init"                      "state1"
#  [3] "state2"                    "obj_fun"
#  [5] "DQs"                       "estimated_values"
#  [7] "SSB"                       "Mortality"
#  [9] "Rec"                       "NaturalMortalityOgive_all"
# [11] "MaturationOgive_male"      "MaturationOgive_female"
# [13] "summerTANSel_male"         "summerTANSel_female"
# [15] "autumnTANSel_male"         "autumnTANSel_female"
# [17] "trwlFSel_male"             "trwlFSel_female"
# [19] "lineHomeFSel_male"         "lineHomeFSel_female"
# [21] "lineSpawnFSel_male"        "lineSpawnFSel_female"
# [23] "Tangaroa_bio_summer"       "Tangaroa_bio_autumn"
# [25] "estimated_summary"
# > names(cas2_mpd_alt2)
#  [1] "Init"                      "state1"
#  [3] "state2"                    "obj_fun"
#  [5] "DQs"                       "estimated_values"
#  [7] "SSB"                       "Mortality"
#  [9] "Rec"                       "NaturalMortalityOgive_all"
# [11] "MaturationOgive_male"      "MaturationOgive_female"
# [13] "summerTANSel_male"         "summerTANSel_female"
# [15] "autumnTANSel_male"         "autumnTANSel_female"
# [17] "trwlFSel_male"             "trwlFSel_female"
# [19] "lineHomeFSel_male"         "lineHomeFSel_female"
# [21] "lineSpawnFSel_male"        "lineSpawnFSel_female"
# [23] "Tangaroa_bio_summer"       "Tangaroa_bio_autumn"
# [25] "estimated_summary"         "warnings_encounted"




# save_par <- par()

# A4 paper landscape is 297 x 210 (11.7 x 8.3)
# pdf(onefile=TRUE, width=11.0, height=7.6, paper='a4r')
pdf(onefile=TRUE, width=7.6, height=11.0, paper='a4')

# c(bottom, left, top, right)
par(mar=c(4,4,2,1) + 0.1)

par(mfrow=c(2,1))


# plot SSB

max_val <- max(c1_quant$SSBs$SSB,
               c1_sens1_quant$SSBs$SSB,
               cas2_mpd_base$SSB$SSB$values,
               cas2_mpd_alt1$SSB$SSB$values,
               cas2_mpd_alt2$SSB$SSB$values,
               cas2_mpd_alt3$SSB$SSB$values)

plot(c1_quant$SSBs$year, c1_quant$SSBs$SSB, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='Biomass (t)', main='SSB comparison')
lines(c1_sens1_quant$SSBs$year, c1_sens1_quant$SSBs$SSB, type='l', col='grey', lwd=3)
lines(names(cas2_mpd_base$SSB$SSB$values), cas2_mpd_base$SSB$SSB$values, col='blue', lwd=1)
lines(names(cas2_mpd_alt1$SSB$SSB$values), cas2_mpd_alt1$SSB$SSB$values, col='green3', lwd=1)
lines(names(cas2_mpd_alt2$SSB$SSB$values), cas2_mpd_alt2$SSB$SSB$values, col='red', lwd=1)
lines(names(cas2_mpd_alt3$SSB$SSB$values), cas2_mpd_alt3$SSB$SSB$values, col='gold', lwd=1)

legend(c1_quant$SSBs$year[1], 0.50*max_val,
       c('CASAL', 'CASAL sens1', 'Casal2 w/CASAL flags on', 'Casal2 w/CASAL flags off', 'Casal2 w/CASAL flags on and low tolerance', 'Casal2 CppAD'),
       lwd=3,
       col=c('black', 'grey', 'blue', 'green3', 'red', 'gold'))



# plot initial numbers

# Q:  how to get CASAL initial age structure?

# Initial state:
# Partition:
#   sex        3         4         5         6         7         8         9         10        11        12        13        14        15        16        17        18        19        20        21        22        23        24        25
#   male       0         2.134e+07 1.53e+07  1.136e+07 8.705e+06 6.86e+06  5.543e+06 4.581e+06 3.863e+06 3.316e+06 2.884e+06 2.505e+06 2.152e+06 1.825e+06 1.528e+06 1.261e+06 1.024e+06 8.184e+05 6.421e+05 4.941e+05 3.722e+05 2.74e+05  6.655e+05
#   female     0         2.134e+07 1.53e+07  1.136e+07 8.705e+06 6.86e+06  5.543e+06 4.581e+06 3.863e+06 3.316e+06 2.884e+06 2.505e+06 2.152e+06 1.825e+06 1.528e+06 1.261e+06 1.024e+06 8.184e+05 6.421e+05 4.941e+05 3.722e+05 2.74e+05  6.655e+05

c1_mat <- matrix(c(0,2.134e+07,1.53e+07,1.136e+07,8.705e+06,6.86e+06,5.543e+06,4.581e+06,3.863e+06,3.316e+06,2.884e+06,2.505e+06,2.152e+06,1.825e+06,1.528e+06,1.261e+06,1.024e+06,8.184e+05,6.421e+05,4.941e+05,3.722e+05,2.74e+05,6.655e+05,
                   0,2.134e+07,1.53e+07,1.136e+07,8.705e+06,6.86e+06,5.543e+06,4.581e+06,3.863e+06,3.316e+06,2.884e+06,2.505e+06,2.152e+06,1.825e+06,1.528e+06,1.261e+06,1.024e+06,8.184e+05,6.421e+05,4.941e+05,3.722e+05,2.74e+05,6.655e+05),
                 nrow=2, ncol=(25 - 3 + 1), byrow=TRUE)


# Initial state:
# Partition:
#   sex        3         4         5         6         7         8         9         10        11        12        13        14        15        16        17        18        19        20        21        22        23        24        25
#   male       0         2.539e+07 1.728e+07 1.231e+07 9.133e+06 7.017e+06 5.561e+06 4.531e+06 3.782e+06 3.225e+06 2.802e+06 2.459e+06 2.132e+06 1.826e+06 1.543e+06 1.284e+06 1.051e+06 8.457e+05 6.674e+05 5.157e+05 3.896e+05 2.87e+05  6.904e+05
#   female     0         2.539e+07 1.728e+07 1.231e+07 9.133e+06 7.017e+06 5.561e+06 4.531e+06 3.782e+06 3.225e+06 2.802e+06 2.459e+06 2.132e+06 1.826e+06 1.543e+06 1.284e+06 1.051e+06 8.457e+05 6.674e+05 5.157e+05 3.896e+05 2.87e+05  6.904e+05

c1_sens1_mat <- matrix(c(0,2.54E+07,1.73E+07,1.23E+07,9.13E+06,7.02E+06,5.56E+06,4.53E+06,3.78E+06,3.23E+06,2.80E+06,2.46E+06,2.13E+06,1.83E+06,1.54E+06,1.28E+06,1.05E+06,8.46E+05,6.67E+05,5.16E+05,3.90E+05,2.87E+05,6.90E+05,
                         0,2.54E+07,1.73E+07,1.23E+07,9.13E+06,7.02E+06,5.56E+06,4.53E+06,3.78E+06,3.23E+06,2.80E+06,2.46E+06,2.13E+06,1.83E+06,1.54E+06,1.28E+06,1.05E+06,8.46E+05,6.67E+05,5.16E+05,3.90E+05,2.87E+05,6.90E+05),
                       nrow=2, ncol=(25 - 3 + 1), byrow=TRUE)


# omit the 'category' column
c2_base_mat <- as.matrix(cas2_mpd_base$Init$values[,-1])
c2_alt1_mat <- as.matrix(cas2_mpd_alt1$Init$values[,-1])
c2_alt2_mat <- as.matrix(cas2_mpd_alt2$Init$values[,-1])
c2_alt3_mat <- as.matrix(cas2_mpd_alt3$Init$values[,-1])

max_val <- max(c1_mat,
               c1_sens1_mat,
               c2_base_mat,
               c2_alt1_mat,
               c2_alt2_mat,
               c2_alt3_mat)

plot(seq(3, 25), c1_mat[1,], type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='', main='Initial numbers-at-age comparison')
lines(seq(3, 25), c1_mat[2,], col='black', lwd=3, lty=3)
lines(seq(3, 25), c1_sens1_mat[1,], col='grey', lwd=3)
lines(seq(3, 25), c1_sens1_mat[2,], col='grey', lwd=3, lty=3)
lines(as.numeric(colnames(c2_base_mat)), c2_base_mat[1,], col='blue', lwd=1)
lines(as.numeric(colnames(c2_base_mat)), c2_base_mat[2,], col='blue', lwd=1, lty=3)
lines(as.numeric(colnames(c2_alt1_mat)), c2_alt1_mat[1,], col='green3', lwd=1)
lines(as.numeric(colnames(c2_alt1_mat)), c2_alt1_mat[2,], col='green3', lwd=1, lty=3)
lines(as.numeric(colnames(c2_alt2_mat)), c2_alt2_mat[1,], col='red', lwd=1)
lines(as.numeric(colnames(c2_alt2_mat)), c2_alt2_mat[2,], col='red', lwd=1, lty=3)
lines(as.numeric(colnames(c2_alt3_mat)), c2_alt3_mat[1,], col='gold', lwd=1)
lines(as.numeric(colnames(c2_alt3_mat)), c2_alt3_mat[2,], col='gold', lwd=1, lty=3)



# plot YCS

max_val <- max(c1_quant$`Vector parameter values`$recruitment.YCS,
               c1_sens1_quant$`Vector parameter values`$recruitment.YCS,
               cas2_mpd_base$Rec$ycs_values,
               cas2_mpd_alt1$Rec$ycs_values,
               cas2_mpd_alt2$Rec$ycs_values,
               cas2_mpd_alt3$Rec$ycs_values)

plot(c1_quant$`Vector parameter values`$recruitment.YCS, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='', main='YCS comparison')
lines(c1_sens1_quant$`Vector parameter values`$recruitment.YCS, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Rec$ycs_values, col='blue', lwd=1)
lines(cas2_mpd_alt1$Rec$ycs_values, col='green3', lwd=1)
lines(cas2_mpd_alt2$Rec$ycs_values, col='red', lwd=1)
lines(cas2_mpd_alt3$Rec$ycs_values, col='gold', lwd=1)



# plot true YCS

max_val <- max(c1_quant$true_YCS$true_YCS,
               c1_sens1_quant$true_YCS$true_YCS,
               cas2_mpd_base$Rec$true_ycs,
               cas2_mpd_alt1$Rec$true_ycs,
               cas2_mpd_alt2$Rec$true_ycs,
               cas2_mpd_alt3$Rec$true_ycs)

plot(c1_quant$true_YCS$year, c1_quant$true_YCS$true_YCS, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='', main='true YCS comparison')
lines(c1_sens1_quant$true_YCS$year, c1_sens1_quant$true_YCS$true_YCS, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Rec$ycs_years, cas2_mpd_base$Rec$true_ycs, col='blue', lwd=1)
lines(cas2_mpd_alt1$Rec$ycs_years, cas2_mpd_alt1$Rec$true_ycs, col='green3', lwd=1)
lines(cas2_mpd_alt2$Rec$ycs_years, cas2_mpd_alt2$Rec$true_ycs, col='red', lwd=1)
lines(cas2_mpd_alt3$Rec$ycs_years, cas2_mpd_alt3$Rec$true_ycs, col='gold', lwd=1)



# plot true natural mortality-at-age

max_val <- max(c1_quant$`Ogive parameter values`$natural_mortality.ogive_all,
               c1_sens1_quant$`Ogive parameter values`$natural_mortality.ogive_all,
               cas2_mpd_base$NaturalMortalityOgive_all$Values,
               cas2_mpd_alt1$NaturalMortalityOgive_all$Values,
               cas2_mpd_alt2$NaturalMortalityOgive_all$Values,
               cas2_mpd_alt3$NaturalMortalityOgive_all$Values)

plot(c1_quant$`Ogive parameter values`$natural_mortality.ogive_all, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Age', ylab='', main='Natural mortality-at-age comparison')
lines(c1_sens1_quant$`Ogive parameter values`$natural_mortality.ogive_all, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$NaturalMortalityOgive_all$Values, col='blue', lwd=1)
lines(cas2_mpd_alt1$NaturalMortalityOgive_all$Values, col='green3', lwd=1)
lines(cas2_mpd_alt2$NaturalMortalityOgive_all$Values, col='red', lwd=1)
lines(cas2_mpd_alt3$NaturalMortalityOgive_all$Values, col='gold', lwd=1)



# plot summer survey
c1_surv      <- cas_mpd$free$`q[tan_sum].q` * unlist(c1_quant$Tangaroa_bio_summer_est)
c1_s1_surv   <- cas_mpd_sens1$free$`q[tan_sum].q` * unlist(c1_sens1_quant$Tangaroa_bio_summer_est)
c2_base_surv <- cas2_mpd_base$Tangaroa_bio_summer$Values
c2_alt1_surv <- cas2_mpd_alt1$Tangaroa_bio_summer$Values
c2_alt2_surv <- cas2_mpd_alt2$Tangaroa_bio_summer$Values
c2_alt3_surv <- cas2_mpd_alt3$Tangaroa_bio_summer$Values

max_val <- max(c1_surv,
               c1_s1_surv,
               (c2_base_surv$observed * exp(1.96 * c2_base_surv$error_value)),
               c2_base_surv$expected,
               c2_alt1_surv$expected,
               c2_alt2_surv$expected,
               c2_alt3_surv$expected)

plot(names(c1_surv), c1_surv, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='Biomass (t)', main='Tangaroa summer survey comparison')
lines(names(c1_s1_surv), c1_s1_surv, type='l', col='grey', lwd=3)
points(c2_base_surv$year, c2_base_surv$observed, pch=20, col='black')
arrows(c2_base_surv$year, (c2_base_surv$observed * exp(-1.96 * c2_base_surv$error_value)), c2_base_surv$year, (c2_base_surv$observed * exp(1.96 * c2_base_surv$error_value)), length=0.05, angle=90, code=3)
points(c2_base_surv$year, c2_base_surv$expected, col='blue', pch=15)
points(c2_alt1_surv$year, c2_alt1_surv$expected, col='green3', pch=16)
points(c2_alt2_surv$year, c2_alt2_surv$expected, col='red', pch=17)
points(c2_alt3_surv$year, c2_alt3_surv$expected, col='gold', pch=18)



# plot autumn survey

c1_surv      <- cas_mpd$free$`q[tan_aut].q` * unlist(c1_quant$Tangaroa_bio_autumn_est)
c1_s1_surv   <- cas_mpd_sens1$free$`q[tan_aut].q` * unlist(c1_sens1_quant$Tangaroa_bio_autumn_est)
c2_base_surv <- cas2_mpd_base$Tangaroa_bio_autumn$Values
c2_alt1_surv <- cas2_mpd_alt1$Tangaroa_bio_autumn$Values
c2_alt2_surv <- cas2_mpd_alt2$Tangaroa_bio_autumn$Values
c2_alt3_surv <- cas2_mpd_alt3$Tangaroa_bio_autumn$Values

max_val <- max(c1_surv,
               c1_s1_surv,
               (c2_base_surv$observed * exp(1.96 * c2_base_surv$error_value)),
               c2_base_surv$expected,
               c2_alt1_surv$expected,
               c2_alt2_surv$expected,
               c2_alt3_surv$expected)

plot(names(c1_surv), c1_surv, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='Biomass (t)', main='Tangaroa autumn survey comparison')
lines(names(c1_s1_surv), c1_s1_surv, type='l', col='grey', lwd=3)
points(c2_base_surv$year, c2_base_surv$observed, pch=20, col='black')
arrows(c2_base_surv$year, (c2_base_surv$observed * exp(-1.96 * c2_base_surv$error_value)), c2_base_surv$year, (c2_base_surv$observed * exp(1.96 * c2_base_surv$error_value)), length=0.05, angle=90, code=3)
points(c2_base_surv$year, c2_base_surv$expected, col='blue', pch=15)
points(c2_alt1_surv$year, c2_alt1_surv$expected, col='green3', pch=16)
points(c2_alt2_surv$year, c2_alt2_surv$expected, col='red', pch=17)
points(c2_alt3_surv$year, c2_alt3_surv$expected, col='gold', pch=18)



# > names(cas2_mpd_base$Mortality)
#  [1] "categories"                         "label"
#  [3] "m"                                  "selectivities"
#  [5] "time_step_ratio"                    "type"
#  [7] "year"                               "fishing_pressure[FishingLineHome]"
#  [9] "catch[FishingLineHome]"             "actual_catch[FishingLineHome]"
# [11] "fishing_pressure[FishingLineSpawn]" "catch[FishingLineSpawn]"
# [13] "actual_catch[FishingLineSpawn]"     "fishing_pressure[FishingTrwl]"
# [15] "catch[FishingTrwl]"                 "actual_catch[FishingTrwl]"

# plot fishing pressures - trawl

max_val <- max(c1_quant$fishing_pressures$trawl,
               c1_sens1_quant$fishing_pressures$trawl,
               cas2_mpd_base$Mortality$`fishing_pressure[FishingTrwl]`,
               cas2_mpd_alt1$Mortality$`fishing_pressure[FishingTrwl]`,
               cas2_mpd_alt2$Mortality$`fishing_pressure[FishingTrwl]`,
               cas2_mpd_alt3$Mortality$`fishing_pressure[FishingTrwl]`)

plot(c1_quant$fishing_pressures$year, c1_quant$fishing_pressures$trawl, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='U', main='Fishing pressure comparison: trawl')
lines(c1_sens1_quant$fishing_pressures$year, c1_sens1_quant$fishing_pressures$trawl, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Mortality$year, cas2_mpd_base$Mortality$`fishing_pressure[FishingTrwl]`, col='blue', lwd=1)
lines(cas2_mpd_alt1$Mortality$year, cas2_mpd_alt1$Mortality$`fishing_pressure[FishingTrwl]`, col='green3', lwd=1)
lines(cas2_mpd_alt2$Mortality$year, cas2_mpd_alt2$Mortality$`fishing_pressure[FishingTrwl]`, col='red', lwd=1)
lines(cas2_mpd_alt3$Mortality$year, cas2_mpd_alt3$Mortality$`fishing_pressure[FishingTrwl]`, col='gold', lwd=1)



# plot fishing pressures - line_home

max_val <- max(c1_quant$fishing_pressures$line_home,
               c1_sens1_quant$fishing_pressures$line_home,
               cas2_mpd_base$Mortality$`fishing_pressure[FishingLineHome]`,
               cas2_mpd_alt1$Mortality$`fishing_pressure[FishingLineHome]`,
               cas2_mpd_alt2$Mortality$`fishing_pressure[FishingLineHome]`,
               cas2_mpd_alt3$Mortality$`fishing_pressure[FishingLineHome]`)

plot(c1_quant$fishing_pressures$year, c1_quant$fishing_pressures$line_home, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='U', main='Fishing pressure comparison: line_home')
lines(c1_sens1_quant$fishing_pressures$year, c1_sens1_quant$fishing_pressures$line_home, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Mortality$year, cas2_mpd_base$Mortality$`fishing_pressure[FishingLineHome]`, col='blue', lwd=1)
lines(cas2_mpd_alt1$Mortality$year, cas2_mpd_alt1$Mortality$`fishing_pressure[FishingLineHome]`, col='green3', lwd=1)
lines(cas2_mpd_alt2$Mortality$year, cas2_mpd_alt2$Mortality$`fishing_pressure[FishingLineHome]`, col='red', lwd=1)
lines(cas2_mpd_alt3$Mortality$year, cas2_mpd_alt3$Mortality$`fishing_pressure[FishingLineHome]`, col='gold', lwd=1)



# plot fishing pressures - line_spawn

max_val <- max(c1_quant$fishing_pressures$line_spawn,
               c1_sens1_quant$fishing_pressures$line_spawn,
               cas2_mpd_base$Mortality$`fishing_pressure[FishingLineSpawn]`,
               cas2_mpd_alt1$Mortality$`fishing_pressure[FishingLineSpawn]`,
               cas2_mpd_alt2$Mortality$`fishing_pressure[FishingLineSpawn]`,
               cas2_mpd_alt3$Mortality$`fishing_pressure[FishingLineSpawn]`)

plot(c1_quant$fishing_pressures$year, c1_quant$fishing_pressures$line_spawn, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='U', main='Fishing pressure comparison: line_spawn')
lines(c1_sens1_quant$fishing_pressures$year, c1_sens1_quant$fishing_pressures$line_spawn, col='grey', lwd=3)
lines(cas2_mpd_base$Mortality$year, cas2_mpd_base$Mortality$`fishing_pressure[FishingLineSpawn]`, col='blue', lwd=1)
lines(cas2_mpd_alt1$Mortality$year, cas2_mpd_alt1$Mortality$`fishing_pressure[FishingLineSpawn]`, col='green3', lwd=1)
lines(cas2_mpd_alt2$Mortality$year, cas2_mpd_alt2$Mortality$`fishing_pressure[FishingLineSpawn]`, col='red', lwd=1)
lines(cas2_mpd_alt3$Mortality$year, cas2_mpd_alt3$Mortality$`fishing_pressure[FishingLineSpawn]`, col='gold', lwd=1)







ages <- seq(3, 25, 1)

max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].male`, c1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].female`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].male`, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].female`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].male`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age', ylab='', main='Summer survey selectivity-at-age comparison')
lines(ages, c1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].female`, col='black', lwd=3, lty=3)

lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].male`, col='grey', lwd=3)
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].female`, col='grey', lwd=3, lty=3)

lines(ages, cas2_mpd_base$summerTANSel_male$Values, col='blue', lwd=1)
lines(ages, cas2_mpd_base$summerTANSel_female$Values, col='blue', lwd=1, lty=3)

lines(ages, cas2_mpd_alt1$summerTANSel_male$Values, col='green3', lwd=1)
lines(ages, cas2_mpd_alt1$summerTANSel_female$Values, col='green3', lwd=1, lty=3)

lines(ages, cas2_mpd_alt2$summerTANSel_male$Values, col='red', lwd=1)
lines(ages, cas2_mpd_alt2$summerTANSel_female$Values, col='red', lwd=1, lty=3)

lines(ages, cas2_mpd_alt3$summerTANSel_male$Values, col='gold', lwd=1)
lines(ages, cas2_mpd_alt3$summerTANSel_female$Values, col='gold', lwd=1, lty=3)



max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].male`, c1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].female`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].male`, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].female`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].male`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age', ylab='', main='Autumn survey selectivity-at-age comparison')
lines(ages, c1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].female`, col='black', lwd=3, lty=3)

lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].male`, col='grey', lwd=3)
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].female`, col='grey', lwd=3, lty=3)

lines(ages, cas2_mpd_base$autumnTANSel_male$Values, col='blue', lwd=1)
lines(ages, cas2_mpd_base$autumnTANSel_female$Values, col='blue', lwd=1, lty=3)

lines(ages, cas2_mpd_alt1$autumnTANSel_male$Values, col='green3', lwd=1)
lines(ages, cas2_mpd_alt1$autumnTANSel_female$Values, col='green3', lwd=1, lty=3)

lines(ages, cas2_mpd_alt2$autumnTANSel_male$Values, col='red', lwd=1)
lines(ages, cas2_mpd_alt2$autumnTANSel_female$Values, col='red', lwd=1, lty=3)

lines(ages, cas2_mpd_alt3$autumnTANSel_male$Values, col='gold', lwd=1)
lines(ages, cas2_mpd_alt3$autumnTANSel_female$Values, col='gold', lwd=1, lty=3)



max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[trawl_sel].male`, c1_quant$`Ogive parameter values`$`selectivity[trawl_sel].female`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[trawl_sel].male`, c1_sens1_quant$`Ogive parameter values`$`selectivity[trawl_sel].female`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[trawl_sel].male`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age',  ylab='',main='Trawl fishery selectivity-at-age comparison')
lines(ages, c1_quant$`Ogive parameter values`$`selectivity[trawl_sel].female`, col='black', lwd=3, lty=3)

lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[trawl_sel].male`, col='grey', lwd=3)
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[trawl_sel].female`, col='grey', lwd=3, lty=3)

lines(ages, cas2_mpd_base$trwlFSel_male$Values, col='blue', lwd=1)
lines(ages, cas2_mpd_base$trwlFSel_female$Values, col='blue', lwd=1, lty=3)

lines(ages, cas2_mpd_alt1$trwlFSel_male$Values, col='green3', lwd=1)
lines(ages, cas2_mpd_alt1$trwlFSel_female$Values, col='green3', lwd=1, lty=3)

lines(ages, cas2_mpd_alt2$trwlFSel_male$Values, col='red', lwd=1)
lines(ages, cas2_mpd_alt2$trwlFSel_female$Values, col='red', lwd=1, lty=3)

lines(ages, cas2_mpd_alt3$trwlFSel_male$Values, col='gold', lwd=1)
lines(ages, cas2_mpd_alt3$trwlFSel_female$Values, col='gold', lwd=1, lty=3)




max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[line_home_sel].male`, c1_quant$`Ogive parameter values`$`selectivity[line_home_sel].female`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[line_home_sel].male`, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_home_sel].female`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[line_home_sel].male`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age',  ylab='',main='Longline home fishery selectivity-at-age comparison')
lines(ages, c1_quant$`Ogive parameter values`$`selectivity[line_home_sel].female`, col='black', lwd=3, lty=3)

lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_home_sel].male`, col='grey', lwd=3)
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_home_sel].female`, col='grey', lwd=3, lty=3)

lines(ages, cas2_mpd_base$lineHomeFSel_male$Values, col='blue', lwd=1)
lines(ages, cas2_mpd_base$lineHomeFSel_female$Values, col='blue', lwd=1, lty=3)

lines(ages, cas2_mpd_alt1$lineHomeFSel_male$Values, col='green3', lwd=1)
lines(ages, cas2_mpd_alt1$lineHomeFSel_female$Values, col='green3', lwd=1, lty=3)

lines(ages, cas2_mpd_alt2$lineHomeFSel_male$Values, col='red', lwd=1)
lines(ages, cas2_mpd_alt2$lineHomeFSel_female$Values, col='red', lwd=1, lty=3)

lines(ages, cas2_mpd_alt3$lineHomeFSel_male$Values, col='gold', lwd=1)
lines(ages, cas2_mpd_alt3$lineHomeFSel_female$Values, col='gold', lwd=1, lty=3)


max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].male`, c1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].female`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].male`, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].female`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].male`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age',  ylab='',main='Longline spawn fishery selectivity-at-age comparison')
lines(ages, c1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].female`, col='black', lwd=3, lty=3)

lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].male`, col='grey', lwd=3)
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].female`, col='grey', lwd=3, lty=3)

lines(ages, cas2_mpd_base$lineSpawnFSel_male$Values, col='blue', lwd=1)
lines(ages, cas2_mpd_base$lineSpawnFSel_female$Values, col='blue', lwd=1, lty=3)

lines(ages, cas2_mpd_alt1$lineSpawnFSel_male$Values, col='green3', lwd=1)
lines(ages, cas2_mpd_alt1$lineSpawnFSel_female$Values, col='green3', lwd=1, lty=3)

lines(ages, cas2_mpd_alt2$lineSpawnFSel_male$Values, col='red', lwd=1)
lines(ages, cas2_mpd_alt2$lineSpawnFSel_female$Values, col='red', lwd=1, lty=3)

lines(ages, cas2_mpd_alt3$lineSpawnFSel_male$Values, col='gold', lwd=1)
lines(ages, cas2_mpd_alt3$lineSpawnFSel_female$Values, col='gold', lwd=1, lty=3)



dev.off()

