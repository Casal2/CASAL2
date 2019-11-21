## Comparison of CASAL and Casal2 models for LIN5/6
# R 3.6.0 on Linux
#

require(here)

require(casal)
require(casal2)

require(ggplot2)


sessionInfo()


# start in subdirectory 'R-libraries' of git repository
base_dir <- '../TestCases/primary/HOK'

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


# plot E SSB

max_val <- max(c1_quant$SSBs$E,
               c1_sens1_quant$SSBs$E,
               cas2_mpd_base$SSB$SSB_E$values,
               cas2_mpd_alt1$SSB$SSB_E$values,
               cas2_mpd_alt2$SSB$SSB_E$values,
               cas2_mpd_alt3$SSB$SSB_E$values)

plot(c1_quant$SSBs$year, c1_quant$SSBs$E, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='Biomass (t)', main='E SSB comparison')
lines(c1_sens1_quant$SSBs$year, c1_sens1_quant$SSBs$E, type='l', col='grey', lwd=3)
lines(names(cas2_mpd_base$SSB$SSB_E$values), cas2_mpd_base$SSB$SSB_E$values, col='blue', lwd=1)
lines(names(cas2_mpd_alt1$SSB$SSB_E$values), cas2_mpd_alt1$SSB$SSB_E$values, col='green3', lwd=1)
lines(names(cas2_mpd_alt2$SSB$SSB_E$values), cas2_mpd_alt2$SSB$SSB_E$values, col='red', lwd=1)
lines(names(cas2_mpd_alt3$SSB$SSB_E$values), cas2_mpd_alt3$SSB$SSB_E$values, col='gold', lwd=1)

legend(c1_quant$SSBs$year[1], 0.50*max_val,
       c('CASAL', 'CASAL sens1', 'Casal2 w/CASAL flags on', 'Casal2 w/CASAL flags off', 'Casal2 w/CASAL flags on and low tolerance', 'Casal2 CppAD'),
       lwd=3,
       col=c('black', 'grey', 'blue', 'green3', 'red', 'gold'))



# plot W SSB

max_val <- max(c1_quant$SSBs$W,
               c1_sens1_quant$SSBs$W,
               cas2_mpd_base$SSB$SSB_W$values,
               cas2_mpd_alt1$SSB$SSB_W$values,
               cas2_mpd_alt2$SSB$SSB_W$values,
               cas2_mpd_alt3$SSB$SSB_W$values)

plot(c1_quant$SSBs$year, c1_quant$SSBs$W, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='Biomass (t)', main='W SSB comparison')
lines(c1_sens1_quant$SSBs$year, c1_sens1_quant$SSBs$W, type='l', col='grey', lwd=3)
lines(names(cas2_mpd_base$SSB$SSB_E$values), cas2_mpd_base$SSB$SSB_W$values, col='blue', lwd=1)
lines(names(cas2_mpd_alt1$SSB$SSB_E$values), cas2_mpd_alt1$SSB$SSB_W$values, col='green3', lwd=1)
lines(names(cas2_mpd_alt2$SSB$SSB_E$values), cas2_mpd_alt2$SSB$SSB_W$values, col='red', lwd=1)
lines(names(cas2_mpd_alt3$SSB$SSB_E$values), cas2_mpd_alt3$SSB$SSB_W$values, col='gold', lwd=1)


# plot initial numbers

# Q:  how to get CASAL initial age structure?

# Initial state:
# Partition:
#   stock      area       1         2         3         4         5         6         7         8         9         10        11        12        13        14        15        16        17
#   W          SA         0         3.896e+07 4.997e+07 9.41e+07  9.069e+07 7.028e+07 4.367e+07 3.325e+07 2.526e+07 1.918e+07 1.457e+07 1.107e+07 8.407e+06 6.386e+06 4.85e+06  3.684e+06 1.164e+07
#   E          CR         0         1.298e+08 8.763e+07 5.824e+07 3.792e+07 2.4e+07   1.458e+07 1.108e+07 8.415e+06 6.391e+06 4.855e+06 3.688e+06 2.801e+06 2.128e+06 1.616e+06 1.227e+06 3.878e+06
#   W          CR         0         3.896e+08 2.663e+08 1.153e+08 3.853e+07 3.512e+06 2.668e+05 1.013e+04 0         0         0         0         0         0         0         0         0
#   W          WC         0         4.328e+06 1.249e+07 4.033e+07 6.046e+07 7.028e+07 6.55e+07  4.987e+07 3.788e+07 2.878e+07 2.186e+07 1.66e+07  1.261e+07 9.579e+06 7.276e+06 5.526e+06 1.746e+07
#   E          CS         0         1.442e+07 2.191e+07 2.496e+07 2.528e+07 2.4e+07   2.188e+07 1.662e+07 1.262e+07 9.587e+06 7.282e+06 5.531e+06 4.201e+06 3.191e+06 2.424e+06 1.841e+06 5.817e+06

c1_mat <- matrix(c(
    0.0, 38960000.0, 49970000.0, 94100000.0, 90690000.0, 70280000.0, 43670000.0, 33250000.0, 25260000.0, 19180000.0, 14570000.0, 11070000.0, 8407000.0, 6386000.0, 4850000.0, 3684000.0, 11640000.0,
    0.0, 129800000.0, 87630000.0, 58240000.0, 37920000.0, 24000000.0, 14580000.0, 11080000.0, 8415000.0, 6391000.0, 4855000.0, 3688000.0, 2801000.0, 2128000.0, 1616000.0, 1227000.0, 3878000.0,
    0.0, 389600000.0, 266300000.0, 115300000.0, 38530000.0, 3512000.0, 266800.0, 10130.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 4328000.0, 12490000.0, 40330000.0, 60460000.0, 70280000.0, 65500000.0, 49870000.0, 37880000.0, 28780000.0, 21860000.0, 16600000.0, 12610000.0, 9579000.0, 7276000.0, 5526000.0, 17460000.0,
    0.0, 14420000.0, 21910000.0, 24960000.0, 25280000.0, 24000000.0, 21880000.0, 16620000.0, 12620000.0, 9587000.0, 7282000.0, 5531000.0, 4201000.0, 3191000.0, 2424000.0, 1841000.0, 5817000.0),
                 nrow=5, ncol=(17 - 1 + 1), byrow=TRUE)


# Initial state:
# Partition:
#   stock      area       1         2         3         4         5         6         7         8         9         10        11        12        13        14        15        16        17
#   W          SA         0         3.896e+07 4.997e+07 9.41e+07  9.069e+07 7.028e+07 4.367e+07 3.325e+07 2.526e+07 1.918e+07 1.457e+07 1.107e+07 8.407e+06 6.386e+06 4.85e+06  3.684e+06 1.164e+07
#   E          CR         0         1.298e+08 8.763e+07 5.824e+07 3.792e+07 2.4e+07   1.458e+07 1.108e+07 8.415e+06 6.391e+06 4.855e+06 3.688e+06 2.801e+06 2.128e+06 1.616e+06 1.227e+06 3.878e+06
#   W          CR         0         3.896e+08 2.663e+08 1.153e+08 3.853e+07 3.512e+06 2.668e+05 1.013e+04 0         0         0         0         0         0         0         0         0
#   W          WC         0         4.328e+06 1.249e+07 4.033e+07 6.046e+07 7.028e+07 6.55e+07  4.987e+07 3.788e+07 2.878e+07 2.186e+07 1.66e+07  1.261e+07 9.579e+06 7.276e+06 5.526e+06 1.746e+07
#   E          CS         0         1.442e+07 2.191e+07 2.496e+07 2.528e+07 2.4e+07   2.188e+07 1.662e+07 1.262e+07 9.587e+06 7.282e+06 5.531e+06 4.201e+06 3.191e+06 2.424e+06 1.841e+06 5.817e+06

c1_sens1_mat <- matrix(c(
    0.0, 38960000.0, 49970000.0, 94100000.0, 90690000.0, 70280000.0, 43670000.0, 33250000.0, 25260000.0, 19180000.0, 14570000.0, 11070000.0, 8407000.0, 6386000.0, 4850000.0, 3684000.0, 11640000.0,
    0.0, 129800000.0, 87630000.0, 58240000.0, 37920000.0, 24000000.0, 14580000.0, 11080000.0, 8415000.0, 6391000.0, 4855000.0, 3688000.0, 2801000.0, 2128000.0, 1616000.0, 1227000.0, 3878000.0,
    0.0, 389600000.0, 266300000.0, 115300000.0, 38530000.0, 3512000.0, 266800.0, 10130.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 4328000.0, 12490000.0, 40330000.0, 60460000.0, 70280000.0, 65500000.0, 49870000.0, 37880000.0, 28780000.0, 21860000.0, 16600000.0, 12610000.0, 9579000.0, 7276000.0, 5526000.0, 17460000.0,
    0.0, 14420000.0, 21910000.0, 24960000.0, 25280000.0, 24000000.0, 21880000.0, 16620000.0, 12620000.0, 9587000.0, 7282000.0, 5531000.0, 4201000.0, 3191000.0, 2424000.0, 1841000.0, 5817000.0),
                       nrow=5, ncol=(17 - 1 + 1), byrow=TRUE)


# omit the 'category' column
# categories: west.sa, east.cr, west.cr, west.wc, east.cs

c2_base_mat <- as.matrix(cas2_mpd_base$Init$values[,-1])
c2_alt1_mat <- as.matrix(cas2_mpd_alt1$Init$values[,-1])
c2_alt2_mat <- as.matrix(cas2_mpd_alt2$Init$values[,-1])
c2_alt3_mat <- as.matrix(cas2_mpd_alt3$Init$values[,-1])


for (i in 1:nrow(c2_base_mat))
{
    max_val <- max(c1_mat[i,],
                   c1_sens1_mat[i,],
                   c2_base_mat[i,],
                   c2_alt1_mat[i,],
                   c2_alt2_mat[i,],
                   c2_alt3_mat[i,])

    plot(seq(1, 17), c1_mat[i,], type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='', main=paste('Initial numbers-at-age comparison:', cas2_mpd_base$Init$values[i,1]))
    lines(seq(1, 17), c1_sens1_mat[i,], col='grey', lwd=3)
    lines(as.numeric(colnames(c2_base_mat)), c2_base_mat[i,], col='blue', lwd=1)
    lines(as.numeric(colnames(c2_alt1_mat)), c2_alt1_mat[i,], col='green3', lwd=1)
    lines(as.numeric(colnames(c2_alt2_mat)), c2_alt2_mat[i,], col='red', lwd=1)
    lines(as.numeric(colnames(c2_alt3_mat)), c2_alt3_mat[i,], col='gold', lwd=1)
}


# plot YCS

max_val <- max(c1_quant$YCS$E,
               c1_sens1_quant$YCS$E,
               cas2_mpd_base$Recruit_E$ycs_values,
               cas2_mpd_alt1$Recruit_E$ycs_values,
               cas2_mpd_alt2$Recruit_E$ycs_values,
               cas2_mpd_alt3$Recruit_E$ycs_values)

plot(c1_quant$YCS$E, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='', main='E YCS comparison')
lines(c1_sens1_quant$YCS$E, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Recruit_E$ycs_values, col='blue', lwd=1)
lines(cas2_mpd_alt1$Recruit_E$ycs_values, col='green3', lwd=1)
lines(cas2_mpd_alt2$Recruit_E$ycs_values, col='red', lwd=1)
lines(cas2_mpd_alt3$Recruit_E$ycs_values, col='gold', lwd=1)


max_val <- max(c1_quant$YCS$W,
               c1_sens1_quant$YCS$W,
               cas2_mpd_base$Recruit_W$ycs_values,
               cas2_mpd_alt1$Recruit_W$ycs_values,
               cas2_mpd_alt2$Recruit_W$ycs_values,
               cas2_mpd_alt3$Recruit_W$ycs_values)

plot(c1_quant$YCS$W, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='', main='W YCS comparison')
lines(c1_sens1_quant$YCS$W, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Recruit_W$ycs_values, col='blue', lwd=1)
lines(cas2_mpd_alt1$Recruit_W$ycs_values, col='green3', lwd=1)
lines(cas2_mpd_alt2$Recruit_W$ycs_values, col='red', lwd=1)
lines(cas2_mpd_alt3$Recruit_W$ycs_values, col='gold', lwd=1)



# plot true YCS

max_val <- max(c1_quant$true_YCS$E,
               c1_sens1_quant$true_YCS$E,
               cas2_mpd_base$Recruit_E$true_ycs,
               cas2_mpd_alt1$Recruit_E$true_ycs,
               cas2_mpd_alt2$Recruit_E$true_ycs,
               cas2_mpd_alt3$Recruit_E$true_ycs)

plot(c1_quant$true_YCS$year, c1_quant$true_YCS$E, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='', main='E true YCS comparison')
lines(c1_sens1_quant$true_YCS$year, c1_sens1_quant$true_YCS$E, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Recruit_E$ycs_years, cas2_mpd_base$Recruit_E$true_ycs, col='blue', lwd=1)
lines(cas2_mpd_alt1$Recruit_E$ycs_years, cas2_mpd_alt1$Recruit_E$true_ycs, col='green3', lwd=1)
lines(cas2_mpd_alt2$Recruit_E$ycs_years, cas2_mpd_alt2$Recruit_E$true_ycs, col='red', lwd=1)
lines(cas2_mpd_alt3$Recruit_E$ycs_years, cas2_mpd_alt3$Recruit_E$true_ycs, col='gold', lwd=1)


max_val <- max(c1_quant$true_YCS$W,
               c1_sens1_quant$true_YCS$W,
               cas2_mpd_base$Recruit_W$true_ycs,
               cas2_mpd_alt1$Recruit_W$true_ycs,
               cas2_mpd_alt2$Recruit_W$true_ycs,
               cas2_mpd_alt3$Recruit_W$true_ycs)

plot(c1_quant$true_YCS$year, c1_quant$true_YCS$W, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='', main='W true YCS comparison')
lines(c1_sens1_quant$true_YCS$year, c1_sens1_quant$true_YCS$W, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Recruit_E$ycs_years, cas2_mpd_base$Recruit_W$true_ycs, col='blue', lwd=1)
lines(cas2_mpd_alt1$Recruit_E$ycs_years, cas2_mpd_alt1$Recruit_W$true_ycs, col='green3', lwd=1)
lines(cas2_mpd_alt2$Recruit_E$ycs_years, cas2_mpd_alt2$Recruit_W$true_ycs, col='red', lwd=1)
lines(cas2_mpd_alt3$Recruit_E$ycs_years, cas2_mpd_alt3$Recruit_W$true_ycs, col='gold', lwd=1)



# plot recruitment

max_val <- max(c1_quant$recruitments$E,
               c1_sens1_quant$recruitments$E,
               cas2_mpd_base$Recruit_E$Recruits,
               cas2_mpd_alt1$Recruit_E$Recruits,
               cas2_mpd_alt2$Recruit_E$Recruits,
               cas2_mpd_alt3$Recruit_E$Recruits)

plot(c1_quant$recruitments$year, c1_quant$recruitments$E, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='', main='E Recruits comparison')
lines(c1_sens1_quant$recruitments$year, c1_sens1_quant$recruitments$E, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Recruit_E$ycs_years, cas2_mpd_base$Recruit_E$Recruits, col='blue', lwd=1)
lines(cas2_mpd_alt1$Recruit_E$ycs_years, cas2_mpd_alt1$Recruit_E$Recruits, col='green3', lwd=1)
lines(cas2_mpd_alt2$Recruit_E$ycs_years, cas2_mpd_alt2$Recruit_E$Recruits, col='red', lwd=1)
lines(cas2_mpd_alt3$Recruit_E$ycs_years, cas2_mpd_alt3$Recruit_E$Recruits, col='gold', lwd=1)


max_val <- max(c1_quant$recruitments$W,
               c1_sens1_quant$recruitments$W,
               cas2_mpd_base$Recruit_W$Recruits,
               cas2_mpd_alt1$Recruit_W$Recruits,
               cas2_mpd_alt2$Recruit_W$Recruits,
               cas2_mpd_alt3$Recruit_W$Recruits)

plot(c1_quant$recruitments$year, c1_quant$recruitments$W, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='', main='W Recruits comparison')
lines(c1_sens1_quant$recruitments$year, c1_sens1_quant$recruitments$W, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Recruit_E$ycs_years, cas2_mpd_base$Recruit_W$Recruits, col='blue', lwd=1)
lines(cas2_mpd_alt1$Recruit_E$ycs_years, cas2_mpd_alt1$Recruit_W$Recruits, col='green3', lwd=1)
lines(cas2_mpd_alt2$Recruit_E$ycs_years, cas2_mpd_alt2$Recruit_W$Recruits, col='red', lwd=1)
lines(cas2_mpd_alt3$Recruit_E$ycs_years, cas2_mpd_alt3$Recruit_W$Recruits, col='gold', lwd=1)




# > names(cas_mpd$free)
#  [1] "q[CSacous].q"
#  [2] "q[WCacous].q"
#  [3] "q[CRsum].q"
#  [4] "q[SAsum].q"
#  [5] "q[SAaut].q"

# > names(c1_quant)
#  [1] "Ogive parameter values" "B0"                     "R0"
#  [4] "SSBs"                   "recruitments"           "YCS"
#  [7] "true_YCS"               "actual_catches"         "fishing_pressures"
# [10] "CRsumbio"               "ESSB"                   "SAbio"
# [13] "WCacous"                "CRsl04"                 "Enspsl04"
# [16] "Espsl04"                "SAsl04"                 "Wnspsl04"
# [19] "Wspsl04"

# > names(cas2_mpd_base)
#  [1] "Init"               "summary"            "objective"
#  [4] "SSB"                "Recruit_E"          "Recruit_W"
#  [7] "Mortality"          "M_male"             "M_female"
# [10] "CSacous"            "WCacous"            "Espage"
# [13] "Wspage"             "Enspage"            "EnspOLF"
# [16] "WnspOLF"            "Wnspage"            "CRsumage"
# [19] "SAsumage"           "SAautage"           "CRsumbio"
# [22] "SAsumbio"           "pspawn_1993"        "pspawn"
# [25] "Enspsl"             "Wnspsl"             "Espsl"
# [28] "Wspsl"              "CRsl"               "SAsl"
# [31] "time_var"           "Qs"                 "Covar"
# [34] "Hess"               "Corr"               "warnings_encounted"

# plot surveys

c1_surv      <- cas_mpd$free$`q[WCacous].q` * unlist(c1_quant$WCacous)
c1_s1_surv   <- cas_mpd_sens1$free$`q[WCacous].q` * unlist(c1_sens1_quant$WCacous)
c2_base_surv <- cas2_mpd_base$WCacous$Values
c2_alt1_surv <- cas2_mpd_alt1$WCacous$Values
c2_alt2_surv <- cas2_mpd_alt2$WCacous$Values
c2_alt3_surv <- cas2_mpd_alt3$WCacous$Values

max_val <- max(c1_surv,
               c1_s1_surv,
               (c2_base_surv$observed * exp(1.96 * c2_base_surv$error_value)),
               c2_base_surv$expected,
               c2_alt1_surv$expected,
               c2_alt2_surv$expected,
               c2_alt3_surv$expected)

plot(names(c1_surv), c1_surv, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='Biomass (t)', main='Survey comparison: WCacous')
lines(names(c1_s1_surv), c1_s1_surv, type='l', col='grey', lwd=3)
points(c2_base_surv$year, c2_base_surv$observed, pch=20, col='black')
arrows(c2_base_surv$year, (c2_base_surv$observed * exp(-1.96 * c2_base_surv$error_value)), c2_base_surv$year, (c2_base_surv$observed * exp(1.96 * c2_base_surv$error_value)), length=0.05, angle=90, code=3)
points(c2_base_surv$year, c2_base_surv$expected, col='blue', pch=15)
points(c2_alt1_surv$year, c2_alt1_surv$expected, col='green3', pch=16)
points(c2_alt2_surv$year, c2_alt2_surv$expected, col='red', pch=17)
points(c2_alt3_surv$year, c2_alt3_surv$expected, col='gold', pch=18)



c1_surv      <- cas_mpd$free$`q[CRsum].q` * unlist(c1_quant$CRsumbio)
c1_s1_surv   <- cas_mpd_sens1$free$`q[CRsum].q` * unlist(c1_sens1_quant$CRsumbio)
c2_base_surv <- cas2_mpd_base$CRsumbio$Values
c2_alt1_surv <- cas2_mpd_alt1$CRsumbio$Values
c2_alt2_surv <- cas2_mpd_alt2$CRsumbio$Values
c2_alt3_surv <- cas2_mpd_alt3$CRsumbio$Values

max_val <- max(c1_surv,
               c1_s1_surv,
               (c2_base_surv$observed * exp(1.96 * c2_base_surv$error_value)),
               c2_base_surv$expected,
               c2_alt1_surv$expected,
               c2_alt2_surv$expected,
               c2_alt3_surv$expected)

plot(names(c1_surv), c1_surv, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='Biomass (t)', main='Survey comparison: CRsumbio')
lines(names(c1_s1_surv), c1_s1_surv, type='l', col='grey', lwd=3)
points(c2_base_surv$year, c2_base_surv$observed, pch=20, col='black')
arrows(c2_base_surv$year, (c2_base_surv$observed * exp(-1.96 * c2_base_surv$error_value)), c2_base_surv$year, (c2_base_surv$observed * exp(1.96 * c2_base_surv$error_value)), length=0.05, angle=90, code=3)
points(c2_base_surv$year, c2_base_surv$expected, col='blue', pch=15)
points(c2_alt1_surv$year, c2_alt1_surv$expected, col='green3', pch=16)
points(c2_alt2_surv$year, c2_alt2_surv$expected, col='red', pch=17)
points(c2_alt3_surv$year, c2_alt3_surv$expected, col='gold', pch=18)



c1_surv      <- cas_mpd$free$`q[SAsum].q` * unlist(c1_quant$SAbio)
c1_s1_surv   <- cas_mpd_sens1$free$`q[SAsum].q` * unlist(c1_sens1_quant$SAbio)
c2_base_surv <- cas2_mpd_base$SAsumbio$Values
c2_alt1_surv <- cas2_mpd_alt1$SAsumbio$Values
c2_alt2_surv <- cas2_mpd_alt2$SAsumbio$Values
c2_alt3_surv <- cas2_mpd_alt3$SAsumbio$Values

max_val <- max(c1_surv,
               c1_s1_surv,
               (c2_base_surv$observed * exp(1.96 * c2_base_surv$error_value)),
               c2_base_surv$expected,
               c2_alt1_surv$expected,
               c2_alt2_surv$expected,
               c2_alt3_surv$expected)

plot(names(c1_surv), c1_surv, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='Biomass (t)', main='Survey comparison: CRsumbio')
lines(names(c1_s1_surv), c1_s1_surv, type='l', col='grey', lwd=3)
points(c2_base_surv$year, c2_base_surv$observed, pch=20, col='black')
arrows(c2_base_surv$year, (c2_base_surv$observed * exp(-1.96 * c2_base_surv$error_value)), c2_base_surv$year, (c2_base_surv$observed * exp(1.96 * c2_base_surv$error_value)), length=0.05, angle=90, code=3)
points(c2_base_surv$year, c2_base_surv$expected, col='blue', pch=15)
points(c2_alt1_surv$year, c2_alt1_surv$expected, col='green3', pch=16)
points(c2_alt2_surv$year, c2_alt2_surv$expected, col='red', pch=17)
points(c2_alt3_surv$year, c2_alt3_surv$expected, col='gold', pch=18)



# > names(c1_quant$fishing_pressures)
# [1] "Ensp1" "Wnsp1" "Ensp2" "Wnsp2" "Esp"   "Wsp"   "year"

# > names(cas2_mpd_base$Mortality)
#  [1] "categories"              "label"                   "m"
#  [4] "selectivities"           "time_step_ratio"         "type"
#  [7] "year"                    "fishing_pressure[Ensp1]" "catch[Ensp1]"
# [10] "actual_catch[Ensp1]"     "fishing_pressure[Ensp2]" "catch[Ensp2]"
# [13] "actual_catch[Ensp2]"     "fishing_pressure[Esp]"   "catch[Esp]"
# [16] "actual_catch[Esp]"       "fishing_pressure[Wnsp1]" "catch[Wnsp1]"
# [19] "actual_catch[Wnsp1]"     "fishing_pressure[Wnsp2]" "catch[Wnsp2]"
# [22] "actual_catch[Wnsp2]"     "fishing_pressure[Wsp]"   "catch[Wsp]"
# [25] "actual_catch[Wsp]"

# plot fishing pressures

max_val <- max(c1_quant$fishing_pressures$Ensp1,
               c1_sens1_quant$fishing_pressures$Ensp1,
               cas2_mpd_base$Mortality$`fishing_pressure[Ensp1]`,
               cas2_mpd_alt1$Mortality$`fishing_pressure[Ensp1]`,
               cas2_mpd_alt2$Mortality$`fishing_pressure[Ensp1]`,
               cas2_mpd_alt3$Mortality$`fishing_pressure[Ensp1]`)

plot(c1_quant$fishing_pressures$year, c1_quant$fishing_pressures$Ensp1, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='U', main='Fishing pressure comparison: Ensp1')
lines(c1_sens1_quant$fishing_pressures$year, c1_sens1_quant$fishing_pressures$Ensp1, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Mortality$year, cas2_mpd_base$Mortality$`fishing_pressure[Ensp1]`, col='blue', lwd=1)
lines(cas2_mpd_alt1$Mortality$year, cas2_mpd_alt1$Mortality$`fishing_pressure[Ensp1]`, col='green3', lwd=1)
lines(cas2_mpd_alt2$Mortality$year, cas2_mpd_alt2$Mortality$`fishing_pressure[Ensp1]`, col='red', lwd=1)
lines(cas2_mpd_alt3$Mortality$year, cas2_mpd_alt3$Mortality$`fishing_pressure[Ensp1]`, col='gold', lwd=1)



max_val <- max(c1_quant$fishing_pressures$Wnsp1,
               c1_sens1_quant$fishing_pressures$Wnsp1,
               cas2_mpd_base$Mortality$`fishing_pressure[Wnsp1]`,
               cas2_mpd_alt1$Mortality$`fishing_pressure[Wnsp1]`,
               cas2_mpd_alt2$Mortality$`fishing_pressure[Wnsp1]`,
               cas2_mpd_alt3$Mortality$`fishing_pressure[Wnsp1]`)

plot(c1_quant$fishing_pressures$year, c1_quant$fishing_pressures$Wnsp1, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='U', main='Fishing pressure comparison: Wnsp1')
lines(c1_sens1_quant$fishing_pressures$year, c1_sens1_quant$fishing_pressures$Wnsp1, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Mortality$year, cas2_mpd_base$Mortality$`fishing_pressure[Wnsp1]`, col='blue', lwd=1)
lines(cas2_mpd_alt1$Mortality$year, cas2_mpd_alt1$Mortality$`fishing_pressure[Wnsp1]`, col='green3', lwd=1)
lines(cas2_mpd_alt2$Mortality$year, cas2_mpd_alt2$Mortality$`fishing_pressure[Wnsp1]`, col='red', lwd=1)
lines(cas2_mpd_alt3$Mortality$year, cas2_mpd_alt3$Mortality$`fishing_pressure[Wnsp1]`, col='gold', lwd=1)


max_val <- max(c1_quant$fishing_pressures$Ensp2,
               c1_sens1_quant$fishing_pressures$Ensp2,
               cas2_mpd_base$Mortality$`fishing_pressure[Ensp2]`,
               cas2_mpd_alt1$Mortality$`fishing_pressure[Ensp2]`,
               cas2_mpd_alt2$Mortality$`fishing_pressure[Ensp2]`,
               cas2_mpd_alt3$Mortality$`fishing_pressure[Ensp2]`)

plot(c1_quant$fishing_pressures$year, c1_quant$fishing_pressures$Ensp2, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='U', main='Fishing pressure comparison: Ensp2')
lines(c1_sens1_quant$fishing_pressures$year, c1_sens1_quant$fishing_pressures$Ensp2, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Mortality$year, cas2_mpd_base$Mortality$`fishing_pressure[Ensp2]`, col='blue', lwd=1)
lines(cas2_mpd_alt1$Mortality$year, cas2_mpd_alt1$Mortality$`fishing_pressure[Ensp2]`, col='green3', lwd=1)
lines(cas2_mpd_alt2$Mortality$year, cas2_mpd_alt2$Mortality$`fishing_pressure[Ensp2]`, col='red', lwd=1)
lines(cas2_mpd_alt3$Mortality$year, cas2_mpd_alt3$Mortality$`fishing_pressure[Ensp2]`, col='gold', lwd=1)



max_val <- max(c1_quant$fishing_pressures$Wnsp2,
               c1_sens1_quant$fishing_pressures$Wnsp2,
               cas2_mpd_base$Mortality$`fishing_pressure[Wnsp2]`,
               cas2_mpd_alt1$Mortality$`fishing_pressure[Wnsp2]`,
               cas2_mpd_alt2$Mortality$`fishing_pressure[Wnsp2]`,
               cas2_mpd_alt3$Mortality$`fishing_pressure[Wnsp2]`)

plot(c1_quant$fishing_pressures$year, c1_quant$fishing_pressures$Wnsp2, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='U', main='Fishing pressure comparison: Wnsp2')
lines(c1_sens1_quant$fishing_pressures$year, c1_sens1_quant$fishing_pressures$Wnsp2, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Mortality$year, cas2_mpd_base$Mortality$`fishing_pressure[Wnsp2]`, col='blue', lwd=1)
lines(cas2_mpd_alt1$Mortality$year, cas2_mpd_alt1$Mortality$`fishing_pressure[Wnsp2]`, col='green3', lwd=1)
lines(cas2_mpd_alt2$Mortality$year, cas2_mpd_alt2$Mortality$`fishing_pressure[Wnsp2]`, col='red', lwd=1)
lines(cas2_mpd_alt3$Mortality$year, cas2_mpd_alt3$Mortality$`fishing_pressure[Wnsp2]`, col='gold', lwd=1)



max_val <- max(c1_quant$fishing_pressures$Esp,
               c1_sens1_quant$fishing_pressures$Esp,
               cas2_mpd_base$Mortality$`fishing_pressure[Esp]`,
               cas2_mpd_alt1$Mortality$`fishing_pressure[Esp]`,
               cas2_mpd_alt2$Mortality$`fishing_pressure[Esp]`,
               cas2_mpd_alt3$Mortality$`fishing_pressure[Esp]`)

plot(c1_quant$fishing_pressures$year, c1_quant$fishing_pressures$Esp, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='U', main='Fishing pressure comparison: Esp')
lines(c1_sens1_quant$fishing_pressures$year, c1_sens1_quant$fishing_pressures$Esp, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Mortality$year, cas2_mpd_base$Mortality$`fishing_pressure[Esp]`, col='blue', lwd=1)
lines(cas2_mpd_alt1$Mortality$year, cas2_mpd_alt1$Mortality$`fishing_pressure[Esp]`, col='green3', lwd=1)
lines(cas2_mpd_alt2$Mortality$year, cas2_mpd_alt2$Mortality$`fishing_pressure[Esp]`, col='red', lwd=1)
lines(cas2_mpd_alt3$Mortality$year, cas2_mpd_alt3$Mortality$`fishing_pressure[Esp]`, col='gold', lwd=1)



max_val <- max(c1_quant$fishing_pressures$Wsp,
               c1_sens1_quant$fishing_pressures$Wsp,
               cas2_mpd_base$Mortality$`fishing_pressure[Wsp]`,
               cas2_mpd_alt1$Mortality$`fishing_pressure[Wsp]`,
               cas2_mpd_alt2$Mortality$`fishing_pressure[Wsp]`,
               cas2_mpd_alt3$Mortality$`fishing_pressure[Wsp]`)

plot(c1_quant$fishing_pressures$year, c1_quant$fishing_pressures$Wsp, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='U', main='Fishing pressure comparison: Wsp')
lines(c1_sens1_quant$fishing_pressures$year, c1_sens1_quant$fishing_pressures$Wsp, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Mortality$year, cas2_mpd_base$Mortality$`fishing_pressure[Wsp]`, col='blue', lwd=1)
lines(cas2_mpd_alt1$Mortality$year, cas2_mpd_alt1$Mortality$`fishing_pressure[Wsp]`, col='green3', lwd=1)
lines(cas2_mpd_alt2$Mortality$year, cas2_mpd_alt2$Mortality$`fishing_pressure[Wsp]`, col='red', lwd=1)
lines(cas2_mpd_alt3$Mortality$year, cas2_mpd_alt3$Mortality$`fishing_pressure[Wsp]`, col='gold', lwd=1)




# > names(c1_quant$`Ogive parameter values`)
# [1] "selectivity[Enspsl].all" "selectivity[Wnspsl].all" "selectivity[Espsl].all"
# [4] "selectivity[Wspsl].all"  "selectivity[CRsl].all"   "selectivity[SAsl].all"

# plot selectivity

ages <- seq(1, 17, 1)


max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[Enspsl].all`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[Enspsl].all`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[Enspsl].all`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age', ylab='', main='Survey selectivity-at-age comparison: Enspsl')
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[Enspsl].all`, col='grey', lwd=3)
lines(ages, cas2_mpd_base$Enspsl$Values, col='blue', lwd=1)
lines(ages, cas2_mpd_alt1$Enspsl$Values, col='green3', lwd=1)
lines(ages, cas2_mpd_alt2$Enspsl$Values, col='red', lwd=1)
lines(ages, cas2_mpd_alt3$Enspsl$Values, col='gold', lwd=1)



max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[Wnspsl].all`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[Wnspsl].all`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[Wnspsl].all`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age', ylab='', main='Survey selectivity-at-age comparison: Wnspsl')
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[Wnspsl].all`, col='grey', lwd=3)
lines(ages, cas2_mpd_base$Wnspsl$Values, col='blue', lwd=1)
lines(ages, cas2_mpd_alt1$Wnspsl$Values, col='green3', lwd=1)
lines(ages, cas2_mpd_alt2$Wnspsl$Values, col='red', lwd=1)
lines(ages, cas2_mpd_alt3$Wnspsl$Values, col='gold', lwd=1)



max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[Espsl].all`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[Espsl].all`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[Espsl].all`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age', ylab='', main='Survey selectivity-at-age comparison: Espsl')
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[Espsl].all`, col='grey', lwd=3)
lines(ages, cas2_mpd_base$Espsl$Values, col='blue', lwd=1)
lines(ages, cas2_mpd_alt1$Espsl$Values, col='green3', lwd=1)
lines(ages, cas2_mpd_alt2$Espsl$Values, col='red', lwd=1)
lines(ages, cas2_mpd_alt3$Espsl$Values, col='gold', lwd=1)



max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[Wspsl].all`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[Wspsl].all`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[Wspsl].all`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age', ylab='', main='Survey selectivity-at-age comparison: Wspsl')
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[Wspsl].all`, col='grey', lwd=3)
lines(ages, cas2_mpd_base$Wspsl$Values, col='blue', lwd=1)
lines(ages, cas2_mpd_alt1$Wspsl$Values, col='green3', lwd=1)
lines(ages, cas2_mpd_alt2$Wspsl$Values, col='red', lwd=1)
lines(ages, cas2_mpd_alt3$Wspsl$Values, col='gold', lwd=1)



max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[CRsl].all`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[CRsl].all`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[CRsl].all`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age', ylab='', main='Survey selectivity-at-age comparison: CRsl')
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[CRsl].all`, col='grey', lwd=3)
lines(ages, cas2_mpd_base$CRsl$Values, col='blue', lwd=1)
lines(ages, cas2_mpd_alt1$CRsl$Values, col='green3', lwd=1)
lines(ages, cas2_mpd_alt2$CRsl$Values, col='red', lwd=1)
lines(ages, cas2_mpd_alt3$CRsl$Values, col='gold', lwd=1)



max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[SAsl].all`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[SAsl].all`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[SAsl].all`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age', ylab='', main='Survey selectivity-at-age comparison: SAsl')
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[SAsl].all`, col='grey', lwd=3)
lines(ages, cas2_mpd_base$SAsl$Values, col='blue', lwd=1)
lines(ages, cas2_mpd_alt1$SAsl$Values, col='green3', lwd=1)
lines(ages, cas2_mpd_alt2$SAsl$Values, col='red', lwd=1)
lines(ages, cas2_mpd_alt3$SAsl$Values, col='gold', lwd=1)




dev.off()



# do the catches differ?

time_series_match <- function(t1_vec, t2_vec)
{
    if (length(t1_vec) == length(t2_vec))
    {
        return (ifelse(max(abs(range(t1_vec - t2_vec))) == 0, 'yes', 'no'))
    }

    return ('no')
}



print(paste('Actual catches for Ensp1 match:',
            time_series_match(c1_quant$actual_catches$Ensp1, cas2_mpd_base$Mortality$`actual_catch[Ensp1]`)))

print(paste('Actual catches for Wnsp1 match:',
            time_series_match(c1_quant$actual_catches$Wnsp1, cas2_mpd_base$Mortality$`actual_catch[Wnsp1]`)))

print(paste('Actual catches for Ensp2 match:',
            time_series_match(c1_quant$actual_catches$Ensp2, cas2_mpd_base$Mortality$`actual_catch[Ensp2]`)))

print(paste('Actual catches for Wnsp2 match:',
            time_series_match(c1_quant$actual_catches$Wnsp2, cas2_mpd_base$Mortality$`actual_catch[Wnsp2]`)))

print(paste('Actual catches for Esp match:',
            time_series_match(c1_quant$actual_catches$Esp, cas2_mpd_base$Mortality$`actual_catch[Esp]`)))

print(paste('Actual catches for Wsp match:',
            time_series_match(c1_quant$actual_catches$Wsp, cas2_mpd_base$Mortality$`actual_catch[Wsp]`)))


