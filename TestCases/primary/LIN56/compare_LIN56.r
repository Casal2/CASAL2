## Comparison of CASAL and Casal2 models for LIN5/6
# R 3.6.0 on Linux
#

require(here)

require(casal)
require(CASAL2)

require(ggplot2)


base_dir <- '../TestCases/primary/LIN56'

C2_base_dir <- 'casal_flags_on'
C2_alt1_dir <- 'casal_flags_off'
C2_alt2_dir <- 'cppad'

# MPD files are params_est.out, run_estimation.txt, and mpd.out for both CASAL and Casal2
params_est <- 'params_est.out'
mpd_run    <- 'run_estimation.txt'
mpd_out    <- 'mpd.out'


# read in CASAL MPD results
cas_mpd <- casal::extract.mpd(mpd_run, file.path(base_dir, 'CASAL'))
c1_quant <- cas_mpd$quantities

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




save_par <- par()

# c(bottom, left, top, right)
par(mar=c(4,4,2,1) + 0.1)

par(mfrow=c(2,2))


# plot SSB

max_val <- max(c1_quant$SSBs$SSB,
                cas2_mpd_base$SSB$`1`$SSB$values,
                cas2_mpd_alt1$SSB$`1`$SSB$values,
                cas2_mpd_alt2$SSB$`1`$SSB$values)

plot(c1_quant$SSBs$year, c1_quant$SSBs$SSB, type='l', col='black', lwd=2, ylim=c(0, max_val), xlab='Year', ylab='Biomass (t)', main='SSB comparison')
lines(names(cas2_mpd_base$SSB$`1`$SSB$values), cas2_mpd_base$SSB$`1`$SSB$values, col='blue', lwd=2)
lines(names(cas2_mpd_alt1$SSB$`1`$SSB$values), cas2_mpd_alt1$SSB$`1`$SSB$values, col='green3', lwd=2)
lines(names(cas2_mpd_alt2$SSB$`1`$SSB$values), cas2_mpd_alt2$SSB$`1`$SSB$values, col='red', lwd=2)

legend(c1_quant$SSBs$year[1], 0.50*max_val,
       c('CASAL', 'Casal2 w/CASAL flags on', 'Casal2 w/CASAL flags off', 'Casal2 CppAD'),
       lwd=c(2, 2, 2, 2),
       col=c('black', 'blue', 'green3', 'red'))



# plot YCS

max_val <- max(c1_quant$`Vector parameter values`$recruitment.YCS,
               cas2_mpd_base$Rec$`1`$ycs_values,
               cas2_mpd_alt1$Rec$`1`$ycs_values,
               cas2_mpd_alt2$Rec$`1`$ycs_values)

plot(c1_quant$`Vector parameter values`$recruitment.YCS, type='l', col='black', lwd=2, ylim=c(0, max_val), xlab='Year', ylab='', main='YCS comparison')
lines(cas2_mpd_base$Rec$`1`$ycs_values, col='blue', lwd=1)
lines(cas2_mpd_alt1$Rec$`1`$ycs_values, col='green3', lwd=1)
lines(cas2_mpd_alt2$Rec$`1`$ycs_values, col='red', lwd=1)



# plot true YCS

max_val <- max(c1_quant$true_YCS$true_YCS,
               cas2_mpd_base$Rec$`1`$true_ycs,
               cas2_mpd_alt1$Rec$`1`$true_ycs,
               cas2_mpd_alt2$Rec$`1`$true_ycs)

plot(c1_quant$true_YCS$year, c1_quant$true_YCS$true_YCS, type='l', col='black', lwd=2, ylim=c(0, max_val), xlab='Year', ylab='', main='true YCS comparison')
lines(cas2_mpd_base$Rec$`1`$ycs_years, cas2_mpd_base$Rec$`1`$true_ycs, col='blue', lwd=1)
lines(cas2_mpd_alt1$Rec$`1`$ycs_years, cas2_mpd_alt1$Rec$`1`$true_ycs, col='green3', lwd=1)
lines(cas2_mpd_alt2$Rec$`1`$ycs_years, cas2_mpd_alt2$Rec$`1`$true_ycs, col='red', lwd=1)



