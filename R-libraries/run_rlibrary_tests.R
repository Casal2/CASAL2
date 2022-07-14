#'
#' This script is run by the GitHub actions Automatic Build calls
#' DONOT modify or delete unless you know what you are doing
#' This checks that the
#' the output from ModelRunner is compatible with the R-library functions
#' 
#' Currently this script lives in CASAL2//R-libraries
#' assumes there is a directory CheckTestModels at top level
#' assumes getwd() == "CASAL2" i.e. top level
#' 
#' TODO: 
#' - Add tabular extracts
#' - Projections
#' - write.csl2 with Casal2 -r
#' - generate.starting.pars with Casal2 -r

#setwd("C://Software//CASAL2")
library(Casal2);

test_dir = file.path("CheckTestModels")

if(!dir.exists(test_dir)) {
  stop("cannot run tests CheckTestModels is not available")
}
#################
##
#################
# -e folders 
dash_e_folders = c("TwoSex", "SBW", "Simple","ComplexTag", "SexedLengthBased")
# -r 
dash_r_folders = c("additional_priors", "agelength_cv_with_LF", "agelengths_data")
# -i or -I 
dash_i_folders = c("SingleSexTagByLength_input", "Complex_input","TwoSex_input")
# -m
dash_m_folders = c("mcmc_start_mpd_mcmc_fixed","mcmc_start_mpd", "mcmc_resume")

#############################################
## These tests will check that extract() functions will work as expected
## will return an error if doesn't work
#############################################
## -e
for(i in 1:length(dash_e_folders)) {
  this_mpd = tryCatch(extract.mpd(path = file.path(test_dir, dash_e_folders[i]), file = "estimate_betadiff.log"), error = function(e){e}, warning=function(w) {w}))
  if(inherits(this_mpd, "error") |  inherits(this_mpd, "warning")) {
    stop(paste0("Could not read 'estimate_betadiff.log', for TestModel ", dash_e_folders[i]))
  }
}
## -r 
for(i in 1:length(dash_r_folders)) {
  this_mpd = tryCatch(extract.mpd(path = file.path(test_dir, dash_r_folders[i]), file = "run.log"), error = function(e){e}, warning=function(w) {w}))
  if(inherits(this_mpd, "error") |  inherits(this_mpd, "warning")) {
    stop(paste0("Could not read 'run.log', for TestModel ", dash_r_folders[i]))
  }
}
## -i
for(i in 1:length(dash_i_folders)) {
  this_mpd = tryCatch(extract.mpd(path = file.path(test_dir, dash_i_folders[i]), file = "run.log"), error = function(e){e}, warning=function(w) {w}))
  if(inherits(this_mpd, "error") |  inherits(this_mpd, "warning")) {
    stop(paste0("Could not read 'run.log', for TestModel ", dash_i_folders[i]))
  }
}
## -m
for(i in 1:length(dash_m_folders)) {
  this_mcmc = tryCatch(extract.mcmc(path = file.path(test_dir, dash_m_folders[i]), samples.file = "samples.1", objectives.file = "objectives.1"), error = function(e){e}, warning=function(w) {w}))
  if(inherits(this_mcmc, "error") |  inherits(this_mcmc, "warning")) {
    stop(paste0("Could not read mcmc output, for TestModel ", dash_m_folders[i]))
  }
}

#############################################
## Test extract.csl2.file
#############################################
all_dirs = list.dirs(path = test_dir)
for(i in 2:length(all_dirs)) {
  all_csl = list.files(path = all_dirs[i], pattern  = ".csl2")
  ## drop config
  ndx_drop = grepl(all_csl, pattern = "onfig")
  all_csl = all_csl[!ndx_drop]
  all_csl = all_csl[all_csl != "fisheries_catches_unsexed.csl2"]
  if(length(all_csl) >= 1) {
    for(j in 1:length(all_csl)) {
      this_file = tryCatch(extract.csl2.file(path = all_dirs[i], file = all_csl[j], quiet  = T), error = function(e){e}, warning=function(w) {w}))
      if(inherits(this_file, "error") |  inherits(this_file, "warning")) {
        stop(paste0("Could not extract.csl2.file, for TestModel ", all_dirs[i], " and file ", all_csl[j]))
      }
    }
  }
}
