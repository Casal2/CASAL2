# print(paste('pandoc installed?', rmarkdown::pandoc_available()))
# print(paste('pandoc version:', rmarkdown::pandoc_version()))

require(plyr)
require(dplyr)
require(huxtable)

require(ggmcmc)
require(coda)
require(gridExtra)
require(ggthemes)
require(rlist)

require(devtools)

# the CASAL and Casal2 R libraries are installed in the TestCases subdirectory
.libPaths(c('../..', .libPaths()))

if (!require(casal))
{
    install.packages('../../R-functions/casal_2.30.tar.gz', repos=NULL)
    library(casal)
    # install('casal_utils')
}

if (!require(Casal2))
{
#     install.packages('../../R-functions/casal2_1.0.tar.gz', repos=NULL)
    install.packages('../../R-functions/Casal2_21.10.tar.gz', repos=NULL)
    library(Casal2)
}

print(date())
print(sessionInfo())

base_dir <- '.'


# MPD files are params_est.out, run_estimation.txt, and mpd.out for both CASAL and Casal2
params_est <- 'params_est.out'
mpd_run    <- 'run_estimation.txt'
mpd_out    <- 'mpd.out'


# MCMC files are run_mcmc.txt for both CASAL and Casal2
mcmc_run        <- 'run_mcmc.txt'

C1_sample_file  <- 'samples.1'
C1_obj_fun_file <- 'objectives.1'

C2_sample_file  <- 'samples.1'
C2_obj_fun_file <- 'objectives.1'


get_convergence_information <- function(filename)
{
    # for BetaDiff
    # case-insensitive check for all strings with 'converge' and without 'check' (for "Convergence check")
    str_vec <- grep('check', grep('converge|fmm|optimise', readLines(filename), ignore.case=TRUE, value=TRUE), value=TRUE, invert=TRUE)

    # for ADOL-C
    if (length(str_vec) == 0)
    {
        # str_vec <- ?????
    }

    # for CppAD
    if (length(str_vec) == 0)
    {
        str_vec <- grep('Number of|EXIT', readLines(filename), value=TRUE)
    }

    return (str_vec)
}

get_num_seq <- function(tot_num, num_to_use, num_to_subsample=1)
{
    num_to_subsample <- max(1, num_to_subsample)
    return (sort(-seq(-(tot_num), -(tot_num - num_to_use + 1), num_to_subsample)))
}

