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



## NOTE:  got to here 2019-11-18


# plot true natural mortality-at-age

max_val <- max(c1_quant$`Ogive parameter values`$natural_mortality.ogive_all,
               c1_sens1_quant$`Ogive parameter values`$natural_mortality.ogive_all,
               cas2_mpd_base$NaturalMortalityOgive_all$`1`$Values,
               cas2_mpd_alt1$NaturalMortalityOgive_all$`1`$Values,
               cas2_mpd_alt2$NaturalMortalityOgive_all$`1`$Values,
               cas2_mpd_alt3$NaturalMortalityOgive_all$`1`$Values)

plot(c1_quant$`Ogive parameter values`$natural_mortality.ogive_all, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Age', ylab='', main='Natural mortality-at-age comparison')
lines(c1_sens1_quant$`Ogive parameter values`$natural_mortality.ogive_all, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$NaturalMortalityOgive_all$`1`$Values, col='blue', lwd=1)
lines(cas2_mpd_alt1$NaturalMortalityOgive_all$`1`$Values, col='green3', lwd=1)
lines(cas2_mpd_alt2$NaturalMortalityOgive_all$`1`$Values, col='red', lwd=1)
lines(cas2_mpd_alt3$NaturalMortalityOgive_all$`1`$Values, col='gold', lwd=1)



# plot summer survey
c1_surv      <- cas_mpd$free$`q[tan_sum].q` * unlist(c1_quant$Tangaroa_bio_summer_est)
c1_s1_surv   <- cas_mpd_sens1$free$`q[tan_sum].q` * unlist(c1_sens1_quant$Tangaroa_bio_summer_est)
c2_base_surv <- cas2_mpd_base$Tangaroa_bio_summer$`1`$Values
c2_alt1_surv <- cas2_mpd_alt1$Tangaroa_bio_summer$`1`$Values
c2_alt2_surv <- cas2_mpd_alt2$Tangaroa_bio_summer$`1`$Values
c2_alt3_surv <- cas2_mpd_alt3$Tangaroa_bio_summer$`1`$Values

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
c2_base_surv <- cas2_mpd_base$Tangaroa_bio_autumn$`1`$Values
c2_alt1_surv <- cas2_mpd_alt1$Tangaroa_bio_autumn$`1`$Values
c2_alt2_surv <- cas2_mpd_alt2$Tangaroa_bio_autumn$`1`$Values
c2_alt3_surv <- cas2_mpd_alt3$Tangaroa_bio_autumn$`1`$Values

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



# plot fishing pressures - trawl

max_val <- max(c1_quant$fishing_pressures$trawl,
               c1_sens1_quant$fishing_pressures$trawl,
               cas2_mpd_base$Mortality$`1`$`fishing_pressure[FishingTrwl]`,
               cas2_mpd_alt1$Mortality$`1`$`fishing_pressure[FishingTrwl]`,
               cas2_mpd_alt2$Mortality$`1`$`fishing_pressure[FishingTrwl]`,
               cas2_mpd_alt3$Mortality$`1`$`fishing_pressure[FishingTrwl]`)

plot(c1_quant$fishing_pressures$year, c1_quant$fishing_pressures$trawl, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='U', main='Fishing pressure comparison: trawl')
lines(c1_sens1_quant$fishing_pressures$year, c1_sens1_quant$fishing_pressures$trawl, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Mortality$`1`$year, cas2_mpd_base$Mortality$`1`$`fishing_pressure[FishingTrwl]`, col='blue', lwd=1)
lines(cas2_mpd_alt1$Mortality$`1`$year, cas2_mpd_alt1$Mortality$`1`$`fishing_pressure[FishingTrwl]`, col='green3', lwd=1)
lines(cas2_mpd_alt2$Mortality$`1`$year, cas2_mpd_alt2$Mortality$`1`$`fishing_pressure[FishingTrwl]`, col='red', lwd=1)
lines(cas2_mpd_alt3$Mortality$`1`$year, cas2_mpd_alt3$Mortality$`1`$`fishing_pressure[FishingTrwl]`, col='gold', lwd=1)



# plot fishing pressures - line_home

max_val <- max(c1_quant$fishing_pressures$line_home,
               c1_sens1_quant$fishing_pressures$line_home,
               cas2_mpd_base$Mortality$`1`$`fishing_pressure[FishingLineHome]`,
               cas2_mpd_alt1$Mortality$`1`$`fishing_pressure[FishingLineHome]`,
               cas2_mpd_alt2$Mortality$`1`$`fishing_pressure[FishingLineHome]`,
               cas2_mpd_alt3$Mortality$`1`$`fishing_pressure[FishingLineHome]`)

plot(c1_quant$fishing_pressures$year, c1_quant$fishing_pressures$line_home, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='U', main='Fishing pressure comparison: line_home')
lines(c1_sens1_quant$fishing_pressures$year, c1_sens1_quant$fishing_pressures$line_home, type='l', col='grey', lwd=3)
lines(cas2_mpd_base$Mortality$`1`$year, cas2_mpd_base$Mortality$`1`$`fishing_pressure[FishingLineHome]`, col='blue', lwd=1)
lines(cas2_mpd_alt1$Mortality$`1`$year, cas2_mpd_alt1$Mortality$`1`$`fishing_pressure[FishingLineHome]`, col='green3', lwd=1)
lines(cas2_mpd_alt2$Mortality$`1`$year, cas2_mpd_alt2$Mortality$`1`$`fishing_pressure[FishingLineHome]`, col='red', lwd=1)
lines(cas2_mpd_alt3$Mortality$`1`$year, cas2_mpd_alt3$Mortality$`1`$`fishing_pressure[FishingLineHome]`, col='gold', lwd=1)



# plot fishing pressures - line_spawn

max_val <- max(c1_quant$fishing_pressures$line_spawn,
               c1_sens1_quant$fishing_pressures$line_spawn,
               cas2_mpd_base$Mortality$`1`$`fishing_pressure[FishingLineSpawn]`,
               cas2_mpd_alt1$Mortality$`1`$`fishing_pressure[FishingLineSpawn]`,
               cas2_mpd_alt2$Mortality$`1`$`fishing_pressure[FishingLineSpawn]`,
               cas2_mpd_alt3$Mortality$`1`$`fishing_pressure[FishingLineSpawn]`)

plot(c1_quant$fishing_pressures$year, c1_quant$fishing_pressures$line_spawn, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='U', main='Fishing pressure comparison: line_spawn')
lines(c1_sens1_quant$fishing_pressures$year, c1_sens1_quant$fishing_pressures$line_spawn, col='grey', lwd=3)
lines(cas2_mpd_base$Mortality$`1`$year, cas2_mpd_base$Mortality$`1`$`fishing_pressure[FishingLineSpawn]`, col='blue', lwd=1)
lines(cas2_mpd_alt1$Mortality$`1`$year, cas2_mpd_alt1$Mortality$`1`$`fishing_pressure[FishingLineSpawn]`, col='green3', lwd=1)
lines(cas2_mpd_alt2$Mortality$`1`$year, cas2_mpd_alt2$Mortality$`1`$`fishing_pressure[FishingLineSpawn]`, col='red', lwd=1)
lines(cas2_mpd_alt3$Mortality$`1`$year, cas2_mpd_alt3$Mortality$`1`$`fishing_pressure[FishingLineSpawn]`, col='gold', lwd=1)







ages <- seq(3, 25, 1)

max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].male`, c1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].female`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].male`, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].female`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].male`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age', ylab='', main='Summer survey selectivity-at-age comparison')
lines(ages, c1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].female`, col='black', lwd=3, lty=3)

lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].male`, col='grey', lwd=3)
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].female`, col='grey', lwd=3, lty=3)

lines(ages, cas2_mpd_base$summerTANSel_male$`1`$Values, col='blue', lwd=1)
lines(ages, cas2_mpd_base$summerTANSel_female$`1`$Values, col='blue', lwd=1, lty=3)

lines(ages, cas2_mpd_alt1$summerTANSel_male$`1`$Values, col='green3', lwd=1)
lines(ages, cas2_mpd_alt1$summerTANSel_female$`1`$Values, col='green3', lwd=1, lty=3)

lines(ages, cas2_mpd_alt2$summerTANSel_male$`1`$Values, col='red', lwd=1)
lines(ages, cas2_mpd_alt2$summerTANSel_female$`1`$Values, col='red', lwd=1, lty=3)

lines(ages, cas2_mpd_alt3$summerTANSel_male$`1`$Values, col='gold', lwd=1)
lines(ages, cas2_mpd_alt3$summerTANSel_female$`1`$Values, col='gold', lwd=1, lty=3)



max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].male`, c1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].female`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].male`, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].female`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].male`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age', ylab='', main='Autumn survey selectivity-at-age comparison')
lines(ages, c1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].female`, col='black', lwd=3, lty=3)

lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].male`, col='grey', lwd=3)
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].female`, col='grey', lwd=3, lty=3)

lines(ages, cas2_mpd_base$autumnTANSel_male$`1`$Values, col='blue', lwd=1)
lines(ages, cas2_mpd_base$autumnTANSel_female$`1`$Values, col='blue', lwd=1, lty=3)

lines(ages, cas2_mpd_alt1$autumnTANSel_male$`1`$Values, col='green3', lwd=1)
lines(ages, cas2_mpd_alt1$autumnTANSel_female$`1`$Values, col='green3', lwd=1, lty=3)

lines(ages, cas2_mpd_alt2$autumnTANSel_male$`1`$Values, col='red', lwd=1)
lines(ages, cas2_mpd_alt2$autumnTANSel_female$`1`$Values, col='red', lwd=1, lty=3)

lines(ages, cas2_mpd_alt3$autumnTANSel_male$`1`$Values, col='gold', lwd=1)
lines(ages, cas2_mpd_alt3$autumnTANSel_female$`1`$Values, col='gold', lwd=1, lty=3)



max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[trawl_sel].male`, c1_quant$`Ogive parameter values`$`selectivity[trawl_sel].female`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[trawl_sel].male`, c1_sens1_quant$`Ogive parameter values`$`selectivity[trawl_sel].female`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[trawl_sel].male`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age',  ylab='',main='Trawl fishery selectivity-at-age comparison')
lines(ages, c1_quant$`Ogive parameter values`$`selectivity[trawl_sel].female`, col='black', lwd=3, lty=3)

lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[trawl_sel].male`, col='grey', lwd=3)
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[trawl_sel].female`, col='grey', lwd=3, lty=3)

lines(ages, cas2_mpd_base$trwlFSel_male$`1`$Values, col='blue', lwd=1)
lines(ages, cas2_mpd_base$trwlFSel_female$`1`$Values, col='blue', lwd=1, lty=3)

lines(ages, cas2_mpd_alt1$trwlFSel_male$`1`$Values, col='green3', lwd=1)
lines(ages, cas2_mpd_alt1$trwlFSel_female$`1`$Values, col='green3', lwd=1, lty=3)

lines(ages, cas2_mpd_alt2$trwlFSel_male$`1`$Values, col='red', lwd=1)
lines(ages, cas2_mpd_alt2$trwlFSel_female$`1`$Values, col='red', lwd=1, lty=3)

lines(ages, cas2_mpd_alt3$trwlFSel_male$`1`$Values, col='gold', lwd=1)
lines(ages, cas2_mpd_alt3$trwlFSel_female$`1`$Values, col='gold', lwd=1, lty=3)




max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[line_home_sel].male`, c1_quant$`Ogive parameter values`$`selectivity[line_home_sel].female`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[line_home_sel].male`, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_home_sel].female`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[line_home_sel].male`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age',  ylab='',main='Longline home fishery selectivity-at-age comparison')
lines(ages, c1_quant$`Ogive parameter values`$`selectivity[line_home_sel].female`, col='black', lwd=3, lty=3)

lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_home_sel].male`, col='grey', lwd=3)
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_home_sel].female`, col='grey', lwd=3, lty=3)

lines(ages, cas2_mpd_base$lineHomeFSel_male$`1`$Values, col='blue', lwd=1)
lines(ages, cas2_mpd_base$lineHomeFSel_female$`1`$Values, col='blue', lwd=1, lty=3)

lines(ages, cas2_mpd_alt1$lineHomeFSel_male$`1`$Values, col='green3', lwd=1)
lines(ages, cas2_mpd_alt1$lineHomeFSel_female$`1`$Values, col='green3', lwd=1, lty=3)

lines(ages, cas2_mpd_alt2$lineHomeFSel_male$`1`$Values, col='red', lwd=1)
lines(ages, cas2_mpd_alt2$lineHomeFSel_female$`1`$Values, col='red', lwd=1, lty=3)

lines(ages, cas2_mpd_alt3$lineHomeFSel_male$`1`$Values, col='gold', lwd=1)
lines(ages, cas2_mpd_alt3$lineHomeFSel_female$`1`$Values, col='gold', lwd=1, lty=3)


max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].male`, c1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].female`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].male`, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].female`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].male`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age',  ylab='',main='Longline spawn fishery selectivity-at-age comparison')
lines(ages, c1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].female`, col='black', lwd=3, lty=3)

lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].male`, col='grey', lwd=3)
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].female`, col='grey', lwd=3, lty=3)

lines(ages, cas2_mpd_base$lineSpawnFSel_male$`1`$Values, col='blue', lwd=1)
lines(ages, cas2_mpd_base$lineSpawnFSel_female$`1`$Values, col='blue', lwd=1, lty=3)

lines(ages, cas2_mpd_alt1$lineSpawnFSel_male$`1`$Values, col='green3', lwd=1)
lines(ages, cas2_mpd_alt1$lineSpawnFSel_female$`1`$Values, col='green3', lwd=1, lty=3)

lines(ages, cas2_mpd_alt2$lineSpawnFSel_male$`1`$Values, col='red', lwd=1)
lines(ages, cas2_mpd_alt2$lineSpawnFSel_female$`1`$Values, col='red', lwd=1, lty=3)

lines(ages, cas2_mpd_alt3$lineSpawnFSel_male$`1`$Values, col='gold', lwd=1)
lines(ages, cas2_mpd_alt3$lineSpawnFSel_female$`1`$Values, col='gold', lwd=1, lty=3)



dev.off()

