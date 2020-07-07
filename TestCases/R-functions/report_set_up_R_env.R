# print(paste('pandoc installed?', rmarkdown::pandoc_available()))
# print(paste('pandoc version:', rmarkdown::pandoc_version()))

require(ggplot2)
require(rlist)

require(devtools)

# the CASAL and Casal2 R libraries are installed in the TestCases subdirectory
.libPaths(c('../..', .libPaths()))

if (!require(casal))
{
    install.packages('../../../R-libraries/casal_2.30.tar.gz', repos=NULL)
    library(casal)
    # install('casal_utils')
}

if (!require(casal2))
{
    install.packages('../../../R-libraries/casal2_1.0.tar.gz', repos=NULL)
    library(casal2)
}

print(date())
print(sessionInfo())

base_dir <- '.'

# MPD files are params_est.out, run_estimation.txt, and mpd.out for both CASAL and Casal2
params_est <- 'params_est.out'
mpd_run    <- 'run_estimation.txt'
mpd_out    <- 'mpd.out'

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

