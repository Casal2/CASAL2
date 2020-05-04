# NOAA-SAMCP
# R 3.6.3, 2020-04-30

require(casal2)
require(here)

current_dir <- here()


# load Casal2 input file templates
pop_csl2_template_df <- extract.csl2.file(file.path(current_dir, 'template.Population.csl2'))
est_csl2_template_df <- extract.csl2.file(file.path(current_dir, 'template.Estimation.csl2'))
obs_csl2_template_df <- extract.csl2.file(file.path(current_dir, 'template.Observation.csl2'))
rep_csl2_template_df <- extract.csl2.file(file.path(current_dir, 'template.Reports.csl2'))



# use BetaDiff version as the default; matches Estimation.csl2
casal2_path <- '../../BuildSystem/bin/linux/release_betadiff/casal2'



samcp_dir <- '../../../../noaa/NOAA-SAMCP/OM/'

load_samcp_data <- function(data_set_num = -1) {
    if (data_set_num < 1) {
        print('ERROR: Data set number must be positive')
    }

    data_set_file <- file.path(samcp_dir, paste('OM', data_set_num, '.RData', sep = ''))

    if (file.access(data_set_file, 4) == 0)
    {
        load(data_set_file, envir = globalenv(), verbose = TRUE)
    } else {
        print(paste('OM[N].RData File for data set', data_set_num, 'does not exist.'))
        print(paste('File', data_set_file))
    }
}


# > load_samcp_data(N)
# Loading objects:
#   sim1
#   dat.input
#   dat.sim1
#   par.sim1
#   survey.sim1
#   survey.sim1.age

# sim1 are the operating model derived quantity values
# dat.sim1 are
# par.sim1 are the operating model parameter values
# survey.sim1 is 'true' survey abundance (numbers) by year
# survey.sim1.age is 'true' survey abundance (numbers) by age and year
# dat.input are the observed values with noise

# > names(sim1)
# [1] "yr"         "SSB"        "abundance"  "biomass.mt" "N.age"      "L.age"
# [7] "L.knum"     "L.mt"       "msy"        "F"          "FAA"
# > names(dat.sim1)
# [1] "L.obs"          "survey.obs"     "L.age.obs"      "survey.age.obs" "q"
# > names(par.sim1)
# [1] "nyr"               "ages"              "cv.L"              "cv.survey"
# [5] "n.L"               "n.survey"          "logR.sd"           "R0"
# [9] "h"                 "M"                 "Linf"              "K"
# [13] "a0"                "a.lw"              "b.lw"              "A50.mat"
# [17] "slope.mat"         "slope.sel"         "A50.sel"           "slope.sel.survey"
# [21] "A50.sel.survey"    "len"               "W.kg"              "W.mt"
# [25] "M.age"             "mat.age"           "proportion.female" "selex"
# [29] "selex.survey"      "N.pr0"             "Phi.0"             "logR.resid"

# > names(dat.input)
# [1] "L.obs"          "survey.obs"     "L.age.obs"      "survey.age.obs" "q"
# [6] "cv.L"           "cv.survey"

# > names(survey.sim1)
# NULL
# > names(survey.sim1.age)
# NULL
# > length(survey.sim1)
# [1] 30
# > length(survey.sim1.age)
# [1] 360
# > dim(survey.sim1.age)
# [1] 30 12

# c(bottom, left, top, right)
par(mar=c(4,4,2,1) + 0.1)

# > plot(dat.sim1$L.obs, type='l', col='black', lwd=2, xlab='Year', ylab='Landings',
#        ylim=c(0, max(dat.sim1$L.obs, sim1$L.mt, dat.input$L.obs)))
# > lines(sim1$L.mt, col='blue', lwd=2)
# > lines(dat.input$L.obs, col='green', lwd=1)

# > plot(log(dat.sim1$L.obs), type='l', col='black', lwd=2, xlab='Year', ylab='Landings (log)',
#        ylim=log(range(c(dat.sim1$L.obs, sim1$L.mt, dat.input$L.obs))))
# > lines(log(sim1$L.mt), col='blue', lwd=2)
# > lines(log(dat.input$L.obs), col='green', lwd=1)


# > plot(dat.sim1$survey.obs, type='l', col='black', lwd=2, xlab='Year', ylab='Survey index',
#        ylim=range(c(dat.sim1$survey.obs, survey.sim1, dat.input$survey.obs)))
# > lines(survey.sim1, col='blue', lwd=2)
# > lines(dat.input$survey.obs, col='green', lwd=1)



# run_dir could be run-specific
run_dir <- current_dir

# loop over operating model data sets
ds_idx <- 1
load_samcp_data(ds_idx)

par.sim1$yr <- 2000 + sim1$yr



pop_csl2_df <- pop_csl2_template_df
est_csl2_df <- est_csl2_template_df
obs_csl2_df <- obs_csl2_template_df
rep_csl2_df <- rep_csl2_template_df



# write age range and year range to Population.csl2
min_year  <-  min(par.sim1$yr)
max_year  <-  max(par.sim1$yr)
num_years <- length(par.sim1$yr)
year_vec  <- seq(min_year, max_year, 1)
pop_csl2_df$model$start_year$value <- as.character(min_year)
pop_csl2_df$model$final_year$value <- as.character(max_year)

pop_csl2_df$model$projection_final_year$value <- as.character(5 + max_year)

min_age <- min(par.sim1$ages)
max_age <- max(par.sim1$ages)
age_vec <- seq(min_age, max_age, 1)
pop_csl2_df$model$min_age$value <- as.character(min_age)
pop_csl2_df$model$max_age$value <- as.character(max_age)

# TODO - stock-recruitment and YCS years
# par.sim1$h, par.sim1$M

pop_csl2_df$`process[Fishing_Mortality]`$Table$catches$year    <- as.character(year_vec)
pop_csl2_df$`process[Fishing_Mortality]`$Table$catches$Fishery <- as.character(dat.input$L.obs)

pop_csl2_df$`catchability[survey_q]`$q$value <- as.character(dat.input$q)


# write anything to Estimation.csl2?
# est_csl2_df

# where to use catch uncertainty?
# dat.input$cv.L


# write dat.input to Observation.csl2
obs_csl2_df$`observation[survey_abundance]`$years$value       <- as.character(year_vec)
obs_csl2_df$`observation[survey_abundance]`$obs$value         <- as.character(dat.input$survey.obs)
obs_csl2_df$`observation[survey_abundance]`$error_value$value <- as.character(dat.input$cv.survey)

obs_csl2_df$`observation[survey_prop_at_age]`$min_age$value      <- as.character(min_age)
obs_csl2_df$`observation[survey_prop_at_age]`$max_age$value      <- as.character(max_age)
obs_csl2_df$`observation[survey_prop_at_age]`$years$value        <- as.character(year_vec)
obs_csl2_df$`observation[survey_prop_at_age]`$Table$obs          <- list()
for (y in 1:num_years) {
    obs_csl2_df$`observation[survey_prop_at_age]`$Table$obs[[y]] <- as.character(dat.input$survey.age.obs[y,])
}
names(obs_csl2_df$`observation[survey_prop_at_age]`$Table$obs) <- as.character(year_vec)
obs_csl2_df$`observation[survey_prop_at_age]`$Table$error_values <- as.character(rep(100, num_years))
names(obs_csl2_df$`observation[survey_prop_at_age]`$Table$error_values) <- year_vec

obs_csl2_df$`observation[fishery_prop_at_age]`$min_age$value      <- as.character(min_age)
obs_csl2_df$`observation[fishery_prop_at_age]`$max_age$value      <- as.character(max_age)
obs_csl2_df$`observation[fishery_prop_at_age]`$years$value        <- as.character(year_vec)
obs_csl2_df$`observation[fishery_prop_at_age]`$Table$obs          <- list()
for (y in 1:num_years) {
    obs_csl2_df$`observation[fishery_prop_at_age]`$Table$obs[[y]] <- as.character(dat.input$L.age.obs[y,])
}
names(obs_csl2_df$`observation[fishery_prop_at_age]`$Table$obs) <- as.character(year_vec)
obs_csl2_df$`observation[fishery_prop_at_age]`$Table$error_values <- as.character(rep(100, num_years))
names(obs_csl2_df$`observation[fishery_prop_at_age]`$Table$error_values) <- year_vec


# write anything to Reports.csl2?
# rep_csl2_df


write.csl2.file(pop_csl2_df, file.path(run_dir, 'Population.csl2'))
write.csl2.file(est_csl2_df, file.path(run_dir, 'Estimation.csl2'))
write.csl2.file(obs_csl2_df, file.path(run_dir, 'Observation.csl2'))
write.csl2.file(rep_csl2_df, file.path(run_dir, 'Reports.csl2'))



cmd_output <- system2(file.path(casal2_path), paste('-e', '-o est_params.out', sep=' '), stdout=TRUE, stderr=TRUE, stdin='', input=NULL, wait=TRUE)

tail(cmd_output)

output_filename <- file.path(run_dir, paste('est_output.', ds_idx, '.txt', sep=''))
writeLines(cmd_output,  output_filename)

ds_mpd <- extract.mpd(output_filename)

# > names(ds_mpd)
#  [1] "Init"                "state1"              "obj_fun"             "DQs"
#  [5] "estimated_values"    "SSB"                 "Mortality"           "Rec"
#  [9] "maturity_ogive"      "survey_selectivity"  "fishery_selectivity" "survey_abundance"
# [13] "Hess"                "Covar"               "Corr"                "estimated_summary"
# [17] "warnings_encounted"


ds_mpd$warnings_encounted



plot.derived_quantities(ds_mpd, 'SSB')
lines(year_vec, sim1$SSB, type='b', col='blue')


plot.fits(ds_mpd, 'survey_abundance')


plot.pressure(ds_mpd, 'Mortality', col='black', ylim=c(0, max(ds_mpd$Mortality$`fishing_pressure[Fishery]`, sim1$F)))
lines(year_vec, sim1$F, col='blue')


# plots S-R, not recruitment time series
plot.recruitment(ds_mpd, 'Recruitment')
points(sim1$SSB, sim1$N.age[,1] / 1000.0, col='blue')


plot.selectivities(ds_mpd, c('fishery_selectivity', 'survey_selectivity', 'maturity_ogive'), col=c('black', 'green', 'red'))
lines(par.sim1$selex, col='black', lwd=2, lty=2)
lines(par.sim1$selex.survey, col='green', lwd=2, lty=2)
lines(par.sim1$mat.age / 2, col='red', lwd=2, lty=2)


# plots exp(rec_dev)
plot.ycs(ds_mpd, 'Recruitment')
lines(year_vec - 1, exp(par.sim1$logR.resid), col='blue')


# TODO: plot growth comparisons, age-length, age-weight, and length-weight
plot(age_vec, ds_mpd$growth_length_at_age$stock$mean_lengths$values, type='b', col='black', xlab='Age', ylab='Length (cm)',
     ylim=c(0, max(ds_mpd$growth_length_at_age$stock$mean_lengths$values, ds_mpd$estimated_values$values$`age_length[age_len_label].Linf`,
                   par.sim1$len / 10.0, par.sim1$Linf / 10.0)))
lines(age_vec, par.sim1$len / 10.0, type='b', col='blue')
abline(h=ds_mpd$estimated_values$values$`age_length[age_len_label].Linf`, col='black')
abline(h=par.sim1$Linf / 10.0, col='blue')


plot(age_vec, ds_mpd$growth_weight_at_age$stock$mean_weights$values, type='b', col='black',
     xlab='Age', ylab='Weight (mt)', ylim=c(0, max(ds_mpd$growth_weight_at_age$stock$mean_weights$values, par.sim1$W.mt)))
lines(age_vec, par.sim1$W.mt, type='b', col='blue')



ds_mpd$estimated_values
