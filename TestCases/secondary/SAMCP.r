# NOAA-SAMCP
# R 3.6.3, 2020-04-30

require(plyr)
require(dplyr)
require(ggplot2)

require(casal2)
require(here)

current_dir <- here()


# use BetaDiff version as the default; matches Estimation.csl2
casal2_path <- normalizePath('../../BuildSystem/bin/linux/release_betadiff/casal2')

samcp_dir <- normalizePath('../../../dev/NOAA-SAMCP/OM/')


load_samcp_data <- function(data_set_num = -1) {
    if (data_set_num < 1) {
        print('ERROR: Data set number must be positive')
    }

    data_set_file <- file.path(samcp_dir, paste('OM', data_set_num, '.RData', sep = ''))

    if (file.access(data_set_file, 4) == 0)
    {
        load(data_set_file, envir = globalenv(), verbose = TRUE)
    } else {
        print(paste('OM[NNN].RData File for data set', data_set_num, 'does not exist.'))
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

# NOTE:
# sim1 are the 'true' derived quantity values
# par.sim1 are the 'true' parameter values
# survey.sim1 is 'true' survey abundance (numbers) by year
# survey.sim1.age is 'true' survey abundance (numbers) by age and year
# dat.sim1 are the 'observed' values for that simulation
# dat.input are the 'observed' values with noise

# > names(sim1)
#  [1] "yr"         "SSB"        "abundance"  "biomass.mt" "N.age"      "L.age"
#  [7] "L.knum"     "L.mt"       "msy"        "F"          "FAA"
# > names(dat.sim1)
# [1] "L.obs"          "survey.obs"     "L.age.obs"      "survey.age.obs" "q"
# > names(par.sim1)
#  [1] "nyr"               "ages"              "cv.L"              "cv.survey"
#  [5] "n.L"               "n.survey"          "logR.sd"           "R0"
#  [9] "h"                 "M"                 "Linf"              "K"
# [13] "a0"                "a.lw"              "b.lw"              "A50.mat"
# [17] "slope.mat"         "slope.sel"         "A50.sel"           "slope.sel.survey"
# [21] "A50.sel.survey"    "len"               "W.kg"              "W.mt"
# [25] "M.age"             "mat.age"           "proportion.female" "selex"
# [29] "selex.survey"      "N.pr0"             "Phi.0"             "logR.resid"
# [33] "yr"

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


generate_c2_files_for_dataset <- function(ds_idx) {

    pop_csl2_df <- pop_csl2_template_df
    est_csl2_df <- est_csl2_template_df
    obs_csl2_df <- obs_csl2_template_df
    rep_csl2_df <- rep_csl2_template_df


    # write age range and year range to Population.csl2

    pop_csl2_df$model$start_year$value <- as.character(min_year - num_ages)
    pop_csl2_df$model$final_year$value <- as.character(max_year)

    pop_csl2_df$model$projection_final_year$value <- as.character(5 + max_year)

    pop_csl2_df$model$min_age$value <- as.character(min_age)
    pop_csl2_df$model$max_age$value <- as.character(max_age)

    # TODO - stock-recruitment and YCS years
    # par.sim1$h, par.sim1$M

    pop_csl2_df$`process[Fishing_Mortality]`$Table$catches$year    <- as.character(full_year_vec)
    pop_csl2_df$`process[Fishing_Mortality]`$Table$catches$Fishery <- as.character(c(rep(0, num_ages), dat.input$L.obs))

    pop_csl2_df$`catchability[survey_q]`$q$value <- as.character(dat.input$q)


    # write anything to Estimation.csl2?
    # est_csl2_df
    # par.sim1$logR.sd

    # where to use catch uncertainty?
    # dat.input$cv.L


    # write dat.input to Observation.csl2
    obs_csl2_df$`observation[survey_abundance]`$years$value        <- as.character(year_vec)
    obs_csl2_df$`observation[survey_abundance]`$Table$obs          <- list()
    for (y in 1:num_years) {
        obs_csl2_df$`observation[survey_abundance]`$Table$obs[[y]] <- c(as.character(dat.input$survey.obs[y]), as.character(dat.input$cv.survey))
    }
    names(obs_csl2_df$`observation[survey_abundance]`$Table$obs)   <- as.character(year_vec)


    obs_csl2_df$`observation[survey_prop_at_age]`$min_age$value      <- as.character(min_age)
    obs_csl2_df$`observation[survey_prop_at_age]`$max_age$value      <- as.character(max_age)
    obs_csl2_df$`observation[survey_prop_at_age]`$years$value        <- as.character(year_vec)
    obs_csl2_df$`observation[survey_prop_at_age]`$Table$obs          <- list()
    for (y in 1:num_years) {
        obs_csl2_df$`observation[survey_prop_at_age]`$Table$obs[[y]] <- as.character(dat.input$survey.age.obs[y,])
    }
    names(obs_csl2_df$`observation[survey_prop_at_age]`$Table$obs)   <- as.character(year_vec)
    obs_csl2_df$`observation[survey_prop_at_age]`$Table$error_values <- as.character(rep(200, num_years))
    names(obs_csl2_df$`observation[survey_prop_at_age]`$Table$error_values) <- year_vec


    obs_csl2_df$`observation[fishery_prop_at_age]`$min_age$value      <- as.character(min_age)
    obs_csl2_df$`observation[fishery_prop_at_age]`$max_age$value      <- as.character(max_age)
    obs_csl2_df$`observation[fishery_prop_at_age]`$years$value        <- as.character(year_vec)
    obs_csl2_df$`observation[fishery_prop_at_age]`$Table$obs          <- list()
    for (y in 1:num_years) {
        obs_csl2_df$`observation[fishery_prop_at_age]`$Table$obs[[y]] <- as.character(dat.input$L.age.obs[y,])
    }
    names(obs_csl2_df$`observation[fishery_prop_at_age]`$Table$obs) <- as.character(year_vec)
    obs_csl2_df$`observation[fishery_prop_at_age]`$Table$error_values <- as.character(rep(200, num_years))
    names(obs_csl2_df$`observation[fishery_prop_at_age]`$Table$error_values) <- year_vec


    # write anything to Reports.csl2?
    # rep_csl2_df


    config_file <- 'config.csl2'
    file.copy(config_file, file.path(run_dir, config_file))

    write.csl2.file(pop_csl2_df, file.path(run_dir, 'Population.csl2'))
    write.csl2.file(est_csl2_df, file.path(run_dir, 'Estimation.csl2'))
    write.csl2.file(obs_csl2_df, file.path(run_dir, 'Observation.csl2'))
    write.csl2.file(rep_csl2_df, file.path(run_dir, 'Reports.csl2'))

}


plot_c2_mpd <- function(ds_mpd) {

    ds_mpd$warnings_encounted


    # c(bottom, left, top, right)
    par(mar=c(4,4,2,1) + 0.1)

    # > plot(dat.sim1$L.obs, type='l', col='black', lwd=2, xlab='Year', ylab='Landings',
    #        ylim=c(0, max(dat.sim1$L.obs, sim1$L.mt, dat.input$L.obs)))
    # > lines(sim1$L.mt, col='cyan2', lwd=2)
    # > lines(dat.input$L.obs, col='green', lwd=1)

    # > plot(log(dat.sim1$L.obs), type='l', col='black', lwd=2, xlab='Year', ylab='Landings (log)',
    #        ylim=log(range(c(dat.sim1$L.obs, sim1$L.mt, dat.input$L.obs))))
    # > lines(log(sim1$L.mt), col='cyan2', lwd=2)
    # > lines(log(dat.input$L.obs), col='green', lwd=1)


    # > plot(dat.sim1$survey.obs, type='l', col='black', lwd=2, xlab='Year', ylab='Survey index',
    #        ylim=range(c(dat.sim1$survey.obs, survey.sim1, dat.input$survey.obs)))
    # > lines(survey.sim1, col='cyan2', lwd=2)
    # > lines(dat.input$survey.obs, col='green', lwd=1)


    plot.derived_quantities(ds_mpd, 'DerivedQuantities')
    lines(year_vec, sim1$SSB, type='b', col='cyan2', pch=16)


    plot.fits(ds_mpd, 'survey_abundance')
    lines(year_vec, survey.sim1, type='b', col='cyan2', pch=16)


    plot.pressure(ds_mpd, 'Mortality', col='black', ylim=c(0, max(ds_mpd$Mortality$`fishing_pressure[Fishery]`, sim1$F)))
    lines(year_vec, sim1$F, type='b', col='cyan2', pch=16)


    # plots S-R, not recruitment time series
    plot.recruitment(ds_mpd, 'Recruitment')
    true_rec_vec <- sim1$N.age[,1]
    points(sim1$SSB, true_rec_vec / 1000.0, col='cyan2', pch=16)


    # recruitment time series
    plot(full_year_vec, ds_mpd$Recruitment$Recruits, type='b', col='black', lwd=2, pch=20, xlab='Year', ylab='Recruits',
         ylim=range(c(ds_mpd$Recruitment$Recruits, true_rec_vec)))
    abline(h=ds_mpd$Recruitment$r0, col='black')
    lines(year_vec, true_rec_vec, type='b', lwd=2, col='cyan2')
    abline(h=par.sim1$R0, col='cyan2')


    plot.selectivities(ds_mpd, c('fishery_selectivity', 'survey_selectivity', 'maturity_ogive'), col=c('black', 'green', 'red'))
    lines(par.sim1$selex, col='black', lwd=2, lty=2)
    lines(par.sim1$selex.survey, col='green', lwd=2, lty=2)
    lines(par.sim1$mat.age, col='red', lwd=2, lty=2)


    # plots exp(rec_dev)
    plot.ycs(ds_mpd, 'Recruitment')
    lines(year_vec - 1, exp(par.sim1$logR.resid), col='cyan2')


    # plot growth comparisons, age-length, age-weight, and length-weight
    # omit the first column which is 'year'
    age_at_length_vec <- unlist(ds_mpd$growth_length_at_age$female$mean_lengths[-1])
    plot(age_vec, age_at_length_vec, type='b', col='black', xlab='Age', ylab='Length (cm)',
         ylim=c(0, max(age_at_length_vec, ds_mpd$estimated_values$values$`age_length[age_len_label].Linf`,
                       par.sim1$len / 10.0, par.sim1$Linf / 10.0)))
    lines(age_vec, par.sim1$len / 10.0, type='b', col='cyan2')
    abline(h=ds_mpd$estimated_values$values$`age_length[age_len_label].Linf`, col='black')
    abline(h=par.sim1$Linf / 10.0, col='cyan2')


    # omit the first column which is 'year'
    weight_at_length_vec <- unlist(ds_mpd$growth_weight_at_age$female$mean_weights[-1])
    plot(age_vec, weight_at_length_vec, type='b', col='black',
         xlab='Age', ylab='Weight (mt)', ylim=c(0, max(weight_at_length_vec, par.sim1$W.mt)))
    lines(age_vec, par.sim1$W.mt, type='b', col='cyan2')



    # ds_mpd$estimated_values

}





# load Casal2 input file templates
pop_csl2_template_df <- extract.csl2.file(file.path(current_dir, 'template.Population.csl2'))
est_csl2_template_df <- extract.csl2.file(file.path(current_dir, 'template.Estimation.csl2'))
obs_csl2_template_df <- extract.csl2.file(file.path(current_dir, 'template.Observation.csl2'))
rep_csl2_template_df <- extract.csl2.file(file.path(current_dir, 'template.Reports.csl2'))



# run_dir could be run-specific
run_dir <- file.path(current_dir, 'runs')
if (!dir.exists(run_dir)) {
    dir.create(run_dir, recursive=TRUE)
}



# loop over operating model data sets
# files are named 'OM[NNN].RData', where NNN is 1 to 160
om_files <- list.files(samcp_dir, 'OM(\\d{1,3}).RData')
num_ds <- length(om_files)

ds_mpd <- list()

for (ds_idx in 1:num_ds) {

    print(paste('Running number', ds_idx))

    load_samcp_data(ds_idx)


    par.sim1$yr <- 2000 + sim1$yr

    min_year  <- min(par.sim1$yr)
    max_year  <- max(par.sim1$yr)
    num_years <- length(par.sim1$yr)
    year_vec  <- seq(min_year, max_year, 1)

    min_age  <- min(par.sim1$ages)
    max_age  <- max(par.sim1$ages)
    age_vec  <- seq(min_age, max_age, 1)
    num_ages <- length(age_vec)

    full_year_vec <- c(seq(min_year - num_ages, min_year - 1, 1), year_vec)


    generate_c2_files_for_dataset(ds_idx)


    setwd(run_dir)

    cmd_output <- system2(casal2_path, args=c('-e', '-o params_est.out'), stdout=TRUE, stderr=TRUE, stdin='', input=NULL, wait=TRUE)

    # tail(cmd_output)

    output_filename <- file.path(run_dir, paste('est_output.', ds_idx, '.txt', sep=''))
    writeLines(cmd_output, output_filename)

    ds_mpd[[ds_idx]] <- extract.mpd(output_filename)

    # > names(ds_mpd)
    #  [1] "Init"                 "state1"               "obj_fun"
    #  [4] "Mortality"            "Recruitment"          "DerivedQuantities"
    #  [7] "estimated_values"     "estimated_summary"    "Qs"
    # [10] "maturity_ogive"       "growth_length_at_age" "growth_weight_at_age"
    # [13] "survey_selectivity"   "fishery_selectivity"  "survey_abundance"
    # [16] "survey_paa"           "fishery_paa"          "Hess"
    # [19] "Covar"                "Corr"                 "warnings_encounted"


    # save simulation-specific values
    ds_mpd[[ds_idx]]$sim.true  <- dat.sim1
    ds_mpd[[ds_idx]]$sim.noise <- dat.input


    setwd(current_dir)

}

saveRDS(ds_mpd, file=file.path(run_dir, 'C2_ds_mpd.rds'), compress='bzip2')



# summarise

SSB_mat    <- matrix(0, nrow=num_ds, ncol=length(ds_mpd[[1]]$DerivedQuantities$SSB$values), dimnames=list(NULL, full_year_vec))
Rec_mat    <- matrix(0, nrow=num_ds, ncol=length(ds_mpd[[1]]$Recruitment$Recruits), dimnames=list(NULL, full_year_vec))
survey_mat <- matrix(0, nrow=num_ds, ncol=length(ds_mpd[[1]]$survey_abundance$Values$expected), dimnames=list(NULL, year_vec))
U_mat      <- matrix(0, nrow=num_ds, ncol=length(ds_mpd[[1]]$Mortality$`fishing_pressure[Fishery]`), dimnames=list(NULL, full_year_vec))
catch_mat  <- matrix(0, nrow=num_ds, ncol=length(ds_mpd[[1]]$Mortality$`catch[Fishery]`), dimnames=list(NULL, full_year_vec))


survey_pd.true  <- matrix(0, nrow=num_ds, ncol=length(year_vec), , dimnames=list(NULL, year_vec))
survey_pd.noise <- matrix(0, nrow=num_ds, ncol=length(year_vec), , dimnames=list(NULL, year_vec))

catch_pd.true  <- matrix(0, nrow=num_ds, ncol=length(year_vec), , dimnames=list(NULL, year_vec))
catch_pd.noise <- matrix(0, nrow=num_ds, ncol=length(year_vec), , dimnames=list(NULL, year_vec))


survey_q <- data.frame()
SB0      <- c()
R0       <- c()


model_years <- 13:42


for (d in 1:num_ds) {
    dsl            <- ds_mpd[[d]]

    SSB_mat[d,]    <- dsl$DerivedQuantities$SSB$values
    Rec_mat[d,]    <- dsl$Recruitment$Recruits
    survey_mat[d,] <- dsl$survey_abundance$Values$expected
    U_mat[d,]      <- dsl$Mortality$`fishing_pressure[Fishery]`
    catch_mat[d,]  <- dsl$Mortality$`catch[Fishery]`


    survey_pd.true[d,]  <- survey_mat[d,] / dsl$sim.true$survey.obs
    survey_pd.noise[d,] <- survey_mat[d,] / dsl$sim.noise$survey.obs

    catch_pd.true[d,]   <- catch_mat[d,model_years] / dsl$sim.true$L.obs
    catch_pd.noise[d,]  <- catch_mat[d,model_years] / dsl$sim.noise$L.obs


    survey_q <- bind_rows(survey_q, data.frame(name=c('Casal2', 'Sim', 'Sim+noise'), value=c(dsl$Qs$survey_q, dsl$sim.true$q, dsl$sim.noise$q)))
    SB0[d]       <- dsl$Recruitment$b0
    R0[d]        <- dsl$Recruitment$r0
}



true_ssb    <- sim1$SSB
true_rec    <- sim1$N.age[,1]
true_survey <- survey.sim1
true_F      <- sim1$F
true_catch  <- sim1$L.mt


# percent difference
SSB_mat.pd    <- 100.0 * ((SSB_mat[,model_years] / true_ssb) - 1)

Rec_mat.pd    <- 100.0 * ((Rec_mat[,model_years] / true_rec) - 1)

survey_mat.pd <- 100.0 * ((survey_mat / survey.sim1) - 1)

catch_mat.pd  <- 100.0 * ((catch_mat[,model_years] / sim1$L.mt) - 1)


survey_pd.true  <- 100.0 * (survey_pd.true - 1)
survey_pd.noise <- 100.0 * (survey_pd.noise - 1)

catch_pd.true   <- 100.0 * (catch_pd.true - 1)
catch_pd.noise  <- 100.0 * (catch_pd.noise - 1)



boxplot(value ~ name, survey_q, xlab='', ylab='', main='Survey catchability')

boxplot(SB0, main='Equilibrium spawning biomass')

boxplot(R0, main='Equilibrium recruits')



boxplot(SSB_mat, main='SSB')
lines(13:42, true_ssb, col='cyan3', lwd=2)

boxplot(Rec_mat, main='Recruits')
lines(13:42, true_rec, col='cyan3', lwd=2)

boxplot(survey_mat, main='Survey index of abundance')
lines(true_survey, col='cyan3', lwd=2)

boxplot(U_mat, main='Exploitation rate')
lines(13:42, true_F, col='cyan3', lwd=2)

boxplot(catch_mat, main='Catch')
lines(13:42, true_catch, col='cyan3', lwd=2)




boxplot(SSB_mat.pd, ylab='Percent difference', main='SSB')
abline(h=0)

boxplot(Rec_mat.pd, ylab='Percent difference', main='Recruits')
abline(h=0)

boxplot(survey_mat.pd, ylab='Percent difference', main='Survey index')
abline(h=0)

boxplot(catch_mat.pd, ylab='Percent difference', main='Catch')
abline(h=0)



boxplot(survey_pd.true, ylab='Percent difference', main='Survey index: sim true')
abline(h=0)

boxplot(survey_pd.noise, ylab='Percent difference', main='Survey index: sim with noise')
abline(h=0)

boxplot(catch_pd.true, ylab='Percent difference', main='Catch: sim true')
abline(h=0)

boxplot(catch_pd.noise, ylab='Percent difference', main='Catch: sim with noise')
abline(h=0)




p <- ggplot(survey_q, aes(x=name, y=value, fill=name)) + geom_violin() + labs(title = 'Survey catchability')
p <- p + theme(legend.position='none')
plot(p)

p <- ggplot(as.data.frame(SB0), aes(x=0, y=SB0, fill=SB0)) + geom_violin() + labs(title = 'SB0')
p <- p + geom_hline(yintercept=(par.sim1$R0 * par.sim1$Phi.0), color='red') + theme(legend.position='none')
plot(p)

p <- ggplot(as.data.frame(R0), aes(x=0, y=R0, fill=R0)) + geom_violin() + labs(title='R0')
p <- p + geom_hline(yintercept=par.sim1$R0, color='red') + theme(legend.position='none')
plot(p)


