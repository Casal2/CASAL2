# find the non-zero-sized MCMC objective function files for the C2 models
C2_subdir <- unlist(sapply(C2_subdir, function(c_dir) if ( file.access(file.path(base_dir, 'Casal2', c_dir, C2_obj_fun_file), 4) == 0 &
                                                           file.size(file.path(base_dir, 'Casal2', c_dir, C2_obj_fun_file)) > 0 )
                                                         { return (c_dir) }, USE.NAMES=FALSE ))


num_C2_models <- length(C2_subdir)


# read in Casal2 MCMC results
cas2_mcmc <- list()
for (c in 1:num_C2_models)
{
    cas2_mcmc_path <- file.path(base_dir, 'Casal2', C2_subdir[c])

    cas2_mcmc[[c]] <- Casal2::extract.mcmc(C2_sample_file, C2_obj_fun_file, cas2_mcmc_path, return_covariance=TRUE)
}

