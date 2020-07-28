## Comparison of CASAL and Casal2 models for LIN5/6
# R 3.6.0 on Linux
#

# install.packages(c('here', 'ggplot2', 'rlist', 'devtools'))


require(here)

require(ggplot2)

require(rlist)


# start in the 'R-libraries' subdirectory of the git repository
curr_dir <- here()


require(devtools)
if (!require(casal))
{
    install.packages('casal_2.30.tar.gz', repos=NULL, type='source')
    # install('casal_utils')
}

if (!require(casal2))
{
    install.packages('casal2_1.0.tar.gz', repos=NULL, type='source')
}



sessionInfo()


base_dir <- '../TestCases/primary/LIN56'

C2_subdir <- c('adolc_casal_flags_on',
               'adolc_casal_flags_off',
               'adolc_casal_flags_on_low_tol',
               'betadiff_casal_flags_on',
               'betadiff_casal_flags_off',
               'betadiff_casal_flags_on_low_tol',
               'cppad_casal_flags_on',
               'cppad_casal_flags_off')

# MPD files are params_est.out, run_estimation.txt, and mpd.out for both CASAL and Casal2
params_est <- 'params_est.out'
mpd_run    <- 'run_estimation.txt'
mpd_out    <- 'mpd.out'


# find the non-zero-sized parameter estimate files for the C2 models
C2_subdir <- unlist(sapply(C2_subdir, function(c_dir) if ( file.access(file.path(base_dir, 'Casal2', c_dir, params_est), 4) == 0 &
                                                           file.size(file.path(base_dir, 'Casal2', c_dir, params_est)) > 0 )
                                                         { return (c_dir) }, USE.NAMES=FALSE ))


num_C2_models <- length(C2_subdir)

C2_color <- c('blue', 'green3', 'red', 'gold', 'magenta', 'cyan', 'brown', 'orange')


# read in CASAL MPD results
# Q:  what to do if one of these, particularly cas_mpd, does not exist?
cas_mpd  <- casal::extract.mpd(mpd_run, file.path(base_dir, 'CASAL'))
c1_quant <- cas_mpd$quantities

cas_mpd_sens1  <- casal::extract.mpd(mpd_run, file.path(base_dir, 'CASAL_sens1'))
c1_sens1_quant <- cas_mpd_sens1$quantities


# > names(cas_mpd)
# [1] "header"               "objective.function"   "parameters.at.bounds"
# [4] "fits"                 "free"                 "bounds"
# [7] "quantities"
# > names(c1_quant)
#  [1] "Scalar parameter values" "Vector parameter values" "Ogive parameter values"
#  [4] "B0"                      "R0"                      "SSBs"
#  [7] "true_YCS"                "actual_catches"          "fishing_pressures"
# [10] "Tangaroa_bio_autumn_est" "Tangaroa_bio_summer_est"


# read in Casal2 MPD results
cas2_mpd <- list()
for (c in 1:num_C2_models)
{
    cas2_mpd[[c]] <- casal2::extract.mpd(mpd_run, file.path(base_dir, 'Casal2', C2_subdir[c]))
}

# > names(cas2_mpd[[1]])
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



# how to specify field_val so that this works?
get_list_max_val <- function(list_obj, field_val)
{
    num_obj <- length(list_obj)

    if (num_obj > 0)
    {
        max_val <- list_obj[[1]]$field_val

        if (num_obj > 1)
        {
            for (i in 2:num_obj)
            {
                if (list_obj[[i]]$field_val > max_val)
                {
                    max_val <- list_obj[[i]]$field_val
                }
            }
        }

        return (max_val)
    }

    return (NULL)
}




# save_par <- par()
save_opt <- options()

# A4 paper landscape is 297 x 210 (11.7 x 8.3)
# pdf(onefile=TRUE, width=11.0, height=7.6, paper='a4r')
pdf(onefile=TRUE, width=7.6, height=11.0, paper='a4')

# c(bottom, left, top, right)
par(mar=c(4,4,2,1) + 0.1)

par(mfrow=c(2,1))


# plot SSB

max_val <- max(c1_quant$SSBs$SSB,
               c1_sens1_quant$SSBs$SSB,
               max(unlist(list.map(cas2_mpd, max(SSB$SSB$values)))))

plot(c1_quant$SSBs$year, c1_quant$SSBs$SSB, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='Biomass (t)', main='SSB comparison')
lines(c1_sens1_quant$SSBs$year, c1_sens1_quant$SSBs$SSB, type='l', col='grey', lwd=3)

for (c in 1:num_C2_models)
{
    lines(names(cas2_mpd[[c]]$SSB$SSB$values), cas2_mpd[[c]]$SSB$SSB$values, col=C2_color[c], lwd=1)
}

legend(c1_quant$SSBs$year[1], 0.50*max_val,
       c('CASAL', 'CASAL sens1', C2_subdir),
       lwd=3,
       col=c('black', 'grey', C2_color))



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


ages <- seq(3, 25)

# omit the 'category' column
c2_mat <- array(0, dim=c(num_C2_models, dim(as.matrix(cas2_mpd[[1]]$Init$values[,-1]))))
for (c in 1:num_C2_models)
{
    c2_mat[c,,] <- as.matrix(cas2_mpd[[c]]$Init$values[,-1])
}

max_val <- max(c1_mat,
               c1_sens1_mat,
               max(c2_mat))

plot(ages, c1_mat[1,], type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='', main='Initial numbers-at-age comparison')
lines(ages, c1_mat[2,], col='black', lwd=3, lty=3)
lines(ages, c1_sens1_mat[1,], col='grey', lwd=3)
lines(ages, c1_sens1_mat[2,], col='grey', lwd=3, lty=3)
for (c in 1:num_C2_models)
{
    lines(ages, c2_mat[c,1,], col=C2_color[c], lwd=1)
    lines(ages, c2_mat[c,2,], col=C2_color[c], lwd=1, lty=3)
}



# plot YCS

max_val <- max(c1_quant$`Vector parameter values`$recruitment.YCS,
               c1_sens1_quant$`Vector parameter values`$recruitment.YCS,
               max(unlist(list.map(cas2_mpd, max(Rec$ycs_values)))))

plot(c1_quant$`Vector parameter values`$recruitment.YCS, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='', main='YCS comparison')
lines(c1_sens1_quant$`Vector parameter values`$recruitment.YCS, type='l', col='grey', lwd=3)
for (c in 1:num_C2_models)
{
    lines(cas2_mpd[[c]]$Rec$ycs_values, col=C2_color[c], lwd=1)
}



# plot true YCS

max_val <- max(c1_quant$true_YCS$true_YCS,
               c1_sens1_quant$true_YCS$true_YCS,
               max(unlist(list.map(cas2_mpd, max(Rec$true_ycs)))))

plot(c1_quant$true_YCS$year, c1_quant$true_YCS$true_YCS, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='', main='true YCS comparison')
lines(c1_sens1_quant$true_YCS$year, c1_sens1_quant$true_YCS$true_YCS, type='l', col='grey', lwd=3)
for (c in 1:num_C2_models)
{
    lines(cas2_mpd[[c]]$Rec$ycs_years, cas2_mpd[[c]]$Rec$true_ycs, col=C2_color[c], lwd=1)
}



# plot true natural mortality-at-age

max_val <- max(c1_quant$`Ogive parameter values`$natural_mortality.ogive_all,
               c1_sens1_quant$`Ogive parameter values`$natural_mortality.ogive_all,
               max(unlist(list.map(cas2_mpd, max(NaturalMortalityOgive_all$Values)))))

plot(c1_quant$`Ogive parameter values`$natural_mortality.ogive_all, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Age', ylab='', main='Natural mortality-at-age comparison')
lines(c1_sens1_quant$`Ogive parameter values`$natural_mortality.ogive_all, type='l', col='grey', lwd=3)
for (c in 1:num_C2_models)
{
    lines(cas2_mpd[[c]]$NaturalMortalityOgive_all$Values, col=C2_color[c], lwd=1)
}



# plot summer survey
c1_surv      <- cas_mpd$free$`q[tan_sum].q` * unlist(c1_quant$Tangaroa_bio_summer_est)
c1_s1_surv   <- cas_mpd_sens1$free$`q[tan_sum].q` * unlist(c1_sens1_quant$Tangaroa_bio_summer_est)

c2_surv <- list()
for (c in 1:num_C2_models)
{
    c2_surv[[c]] <- cas2_mpd[[c]]$Tangaroa_bio_summer$Values
}

max_val <- max(c1_surv,
               c1_s1_surv,
               (c2_surv[[1]]$observed * exp(1.96 * c2_surv[[1]]$error_value)),
               max(unlist(list.map(c2_surv, max(expected)))))

plot(names(c1_surv), c1_surv, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='Biomass (t)', main='Tangaroa summer survey comparison')
lines(names(c1_s1_surv), c1_s1_surv, type='l', col='grey', lwd=3)
points(c2_surv[[1]]$year, c2_surv[[1]]$observed, pch=20, col='black')
arrows(c2_surv[[1]]$year, (c2_surv[[1]]$observed * exp(-1.96 * c2_surv[[1]]$error_value)), c2_surv[[1]]$year, (c2_surv[[1]]$observed * exp(1.96 * c2_surv[[1]]$error_value)), length=0.05, angle=90, code=3)
for (c in 1:num_C2_models)
{
    points(c2_surv[[c]]$year, c2_surv[[c]]$expected, col=C2_color[c], pch=(15+c-1))
}



# plot autumn survey

c1_surv      <- cas_mpd$free$`q[tan_aut].q` * unlist(c1_quant$Tangaroa_bio_autumn_est)
c1_s1_surv   <- cas_mpd_sens1$free$`q[tan_aut].q` * unlist(c1_sens1_quant$Tangaroa_bio_autumn_est)
c2_surv <- list()
for (c in 1:num_C2_models)
{
    c2_surv[[c]] <- cas2_mpd[[c]]$Tangaroa_bio_autumn$Values
}

max_val <- max(c1_surv,
               c1_s1_surv,
               (c2_surv[[1]]$observed * exp(1.96 * c2_surv[[1]]$error_value)),
               max(unlist(list.map(c2_surv, max(expected)))))

plot(names(c1_surv), c1_surv, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='Biomass (t)', main='Tangaroa autumn survey comparison')
lines(names(c1_s1_surv), c1_s1_surv, type='l', col='grey', lwd=3)
points(c2_surv[[1]]$year, c2_surv[[1]]$observed, pch=20, col='black')
arrows(c2_surv[[1]]$year, (c2_surv[[1]]$observed * exp(-1.96 * c2_surv[[1]]$error_value)), c2_surv[[1]]$year, (c2_surv[[1]]$observed * exp(1.96 * c2_surv[[1]]$error_value)), length=0.05, angle=90, code=3)
for (c in 1:num_C2_models)
{
    points(c2_surv[[c]]$year, c2_surv[[c]]$expected, col=C2_color[c], pch=(15+c-1))
}



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
               max(unlist(list.map(cas2_mpd, max(Mortality$`fishing_pressure[FishingTrwl]`)))))

plot(c1_quant$fishing_pressures$year, c1_quant$fishing_pressures$trawl, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='U', main='Fishing pressure comparison: trawl')
lines(c1_sens1_quant$fishing_pressures$year, c1_sens1_quant$fishing_pressures$trawl, type='l', col='grey', lwd=3)
for (c in 1:num_C2_models)
{
    lines(cas2_mpd[[c]]$Mortality$year, cas2_mpd[[c]]$Mortality$`fishing_pressure[FishingTrwl]`, col=C2_color[c], lwd=1)
}



# plot fishing pressures - line_home

max_val <- max(c1_quant$fishing_pressures$line_home,
               c1_sens1_quant$fishing_pressures$line_home,
               max(unlist(list.map(cas2_mpd, max(Mortality$`fishing_pressure[FishingLineHome]`)))))

plot(c1_quant$fishing_pressures$year, c1_quant$fishing_pressures$line_home, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='U', main='Fishing pressure comparison: line_home')
lines(c1_sens1_quant$fishing_pressures$year, c1_sens1_quant$fishing_pressures$line_home, type='l', col='grey', lwd=3)
for (c in 1:num_C2_models)
{
    lines(cas2_mpd[[c]]$Mortality$year, cas2_mpd[[c]]$Mortality$`fishing_pressure[FishingLineHome]`, col=C2_color[c], lwd=1)
}



# plot fishing pressures - line_spawn

max_val <- max(c1_quant$fishing_pressures$line_spawn,
               c1_sens1_quant$fishing_pressures$line_spawn,
               max(unlist(list.map(cas2_mpd, max(Mortality$`fishing_pressure[FishingLineSpawn]`)))))

plot(c1_quant$fishing_pressures$year, c1_quant$fishing_pressures$line_spawn, type='l', col='black', lwd=3, ylim=c(0, max_val), xlab='Year', ylab='U', main='Fishing pressure comparison: line_spawn')
lines(c1_sens1_quant$fishing_pressures$year, c1_sens1_quant$fishing_pressures$line_spawn, col='grey', lwd=3)
for (c in 1:num_C2_models)
{
    lines(cas2_mpd[[c]]$Mortality$year, cas2_mpd[[c]]$Mortality$`fishing_pressure[FishingLineSpawn]`, col=C2_color[c], lwd=1)
}







ages <- seq(3, 25, 1)

max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].male`, c1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].female`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].male`, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].female`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].male`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age', ylab='', main='Summer survey selectivity-at-age comparison')
lines(ages, c1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].female`, col='black', lwd=3, lty=3)

lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].male`, col='grey', lwd=3)
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveysum_sel].female`, col='grey', lwd=3, lty=3)

for (c in 1:num_C2_models)
{
    lines(ages, cas2_mpd[[c]]$summerTANSel_male$Values, col=C2_color[c], lwd=1)
    lines(ages, cas2_mpd[[c]]$summerTANSel_female$Values, col=C2_color[c], lwd=1, lty=3)
}



max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].male`, c1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].female`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].male`, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].female`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].male`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age', ylab='', main='Autumn survey selectivity-at-age comparison')
lines(ages, c1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].female`, col='black', lwd=3, lty=3)

lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].male`, col='grey', lwd=3)
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[surveyaut_sel].female`, col='grey', lwd=3, lty=3)

for (c in 1:num_C2_models)
{
    lines(ages, cas2_mpd[[c]]$autumnTANSel_male$Values, col=C2_color[c], lwd=1)
    lines(ages, cas2_mpd[[c]]$autumnTANSel_female$Values, col=C2_color[c], lwd=1, lty=3)
}



max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[trawl_sel].male`, c1_quant$`Ogive parameter values`$`selectivity[trawl_sel].female`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[trawl_sel].male`, c1_sens1_quant$`Ogive parameter values`$`selectivity[trawl_sel].female`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[trawl_sel].male`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age',  ylab='',main='Trawl fishery selectivity-at-age comparison')
lines(ages, c1_quant$`Ogive parameter values`$`selectivity[trawl_sel].female`, col='black', lwd=3, lty=3)

lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[trawl_sel].male`, col='grey', lwd=3)
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[trawl_sel].female`, col='grey', lwd=3, lty=3)

for (c in 1:num_C2_models)
{
    lines(ages, cas2_mpd[[c]]$trwlFSel_male$Values, col=C2_color[c], lwd=1)
    lines(ages, cas2_mpd[[c]]$trwlFSel_female$Values, col=C2_color[c], lwd=1, lty=3)
}



max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[line_home_sel].male`, c1_quant$`Ogive parameter values`$`selectivity[line_home_sel].female`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[line_home_sel].male`, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_home_sel].female`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[line_home_sel].male`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age',  ylab='',main='Longline home fishery selectivity-at-age comparison')
lines(ages, c1_quant$`Ogive parameter values`$`selectivity[line_home_sel].female`, col='black', lwd=3, lty=3)

lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_home_sel].male`, col='grey', lwd=3)
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_home_sel].female`, col='grey', lwd=3, lty=3)

for (c in 1:num_C2_models)
{
    lines(ages, cas2_mpd[[c]]$lineHomeFSel_male$Values, col=C2_color[c], lwd=1)
    lines(ages, cas2_mpd[[c]]$lineHomeFSel_female$Values, col=C2_color[c], lwd=1, lty=3)
}



max_val <- max(1,
               c1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].male`, c1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].female`,
               c1_sens1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].male`, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].female`)
plot(ages, c1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].male`, type='l', col='black', lwd=3, ylim=c(0,max_val), xlab='Age',  ylab='',main='Longline spawn fishery selectivity-at-age comparison')
lines(ages, c1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].female`, col='black', lwd=3, lty=3)

lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].male`, col='grey', lwd=3)
lines(ages, c1_sens1_quant$`Ogive parameter values`$`selectivity[line_spawn_sel].female`, col='grey', lwd=3, lty=3)

for (c in 1:num_C2_models)
{
    lines(ages, cas2_mpd[[c]]$lineSpawnFSel_male$Values, col=C2_color[c], lwd=1)
    lines(ages, cas2_mpd[[c]]$lineSpawnFSel_female$Values, col=C2_color[c], lwd=1, lty=3)
}





dev.off()



# do the catch time series differ?

time_series_match <- function(t1_vec, t2_vec)
{
    if (length(t1_vec) == length(t2_vec))
    {
        return (ifelse(max(abs(range(t1_vec - t2_vec))) == 0, 'yes', 'no'))
    }

    return ('no')
}



print('')
for (c in 1:num_C2_models)
{
    print(paste('Catch time series base model comparison for run', C2_subdir[c]))

    print(paste('Actual catches for trawl match:',
                time_series_match(c1_quant$actual_catches$trawl, cas2_mpd[[c]]$Mortality$`actual_catch[FishingTrwl]`)))

    print(paste('Actual catches for line_home match:',
                time_series_match(c1_quant$actual_catches$line_home, cas2_mpd[[c]]$Mortality$`actual_catch[FishingLineHome]`)))

    print(paste('Actual catches for line_spawn match:',
                time_series_match(c1_quant$actual_catches$line_spawn, cas2_mpd[[c]]$Mortality$`actual_catch[FishingLineSpawn]`)))

    print('')
}
print('')




# tables for estimated parameters and objective function values


require(plyr)
require(dplyr)
require(huxtable)


rotate_and_label_cols <- function(df_1, labels)
{
    loc_df <- t(df_1)
    colnames(loc_df) <- labels

    return (loc_df)
}


reformat_to_huxtable <- function(df_1, caption_str=NA, format_num=3, reformat_col=FALSE, col_num=-1, add_rownames=TRUE)
{
    hux_table <- as_hux(df_1, add_colnames=TRUE, add_rownames=add_rownames)

    bottom_border(hux_table)[1,] <- 1
    left_border(hux_table)[,2] <- 1
    caption(hux_table) <- caption_str

    # reformat numbers in table except for the top row and left column
    if (format_num >= 0)
    {
        hux_table <- set_number_format(hux_table, -1, -1, format_num)
    }

    # to reformat and highlight specific columns
    if (reformat_col & col_num != 0)
    {
        hux_table <- set_number_format(hux_table, -1, col_num, 3)
        align(hux_table)[-1,col_num] <- '.'
        hux_table <- map_background_color(hux_table, -1, col_num,
                                          by_ranges(c(-20, -10, 0, 10, 20), c('orange', 'pink', 'white', 'white', 'cyan', 'yellow')))
    }

    return (hux_table)
}


C2_est_params <- cas2_mpd[[1]]$estimated_values$values
c2_est_params_1 <- C2_est_params
C2_obj_fun    <- cas2_mpd[[1]]$obj_fun$values

C2_pd_est_params <- C2_est_params - C2_est_params

if (num_C2_models > 1)
{
    for (c in 2:num_C2_models)
    {
        c2_est_params_c <- cas2_mpd[[c]]$estimated_values$values
        C2_est_params <- bind_rows(C2_est_params, c2_est_params_c)

        C2_pd_est_params <- bind_rows(C2_pd_est_params, 100.0 * (1.0 - (c2_est_params_c / c2_est_params_1)))

        C2_obj_fun    <- bind_rows(C2_obj_fun, cas2_mpd[[c]]$obj_fun$values)
    }
}


options(scipen=999)

print('')

C1_est_params <- cbind(unlist(cas_mpd$free), unlist(cas_mpd_sens1$free), 100.0 * (1.0 - (unlist(cas_mpd_sens1$free) / unlist(cas_mpd$free))))
colnames(C1_est_params) <- c('Base_Model', 'Sensitivity_1', 'Percent_Diff')
print('CASAL parameter estimates')
print(C1_est_params, digits=5)
hux_C1_est_params <- reformat_to_huxtable(C1_est_params, 'CASAL parameter estimates', 5, TRUE, 4)
print('')


C2_est_params <- rotate_and_label_cols(C2_est_params, C2_subdir)
print('Casal2 parameter estimates')
print(C2_est_params, digits=5)
hux_C2_est_params <- reformat_to_huxtable(C2_est_params, 'Casal2 parameter estimates')
print('')


C2_pd_est_params <- rotate_and_label_cols(C2_pd_est_params, C2_subdir)
print('Casal2 parameter estimate percent differences')
print(C2_pd_est_params, digits=3)
hux_C2_pd_est_params <- reformat_to_huxtable(C2_pd_est_params, 'Casal2 parameter estimates: Percent Difference', 3, TRUE, -1)
print('')


C1_obj_fun <- data.frame(Component=cas_mpd$objective.function$components$label,
                         Base_Model=cas_mpd$objective.function$components$value,
                         Sensitivity_1=cas_mpd_sens1$objective.function$components$value)
C1_obj_fun <- bind_rows(C1_obj_fun,
                        data.frame(Component='Total', Base_Model=cas_mpd$objective.function$value, Sensitivity_1=cas_mpd_sens1$objective.function$value))
print('CASAL objective function values')
print(C1_obj_fun, digits=5)
hux_C1_obj_fun <- reformat_to_huxtable(C1_obj_fun, 'CASAL objective function values', add_rownames=FALSE)
print('')


C2_obj_fun <- rotate_and_label_cols(C2_obj_fun, C2_subdir)
print('Casal2 objective function values')
print(C2_obj_fun, digits=5)
hux_C2_obj_fun <- reformat_to_huxtable(C2_obj_fun, 'Casal2 objective function values')
print('')

print('')

options(scipen=0)

