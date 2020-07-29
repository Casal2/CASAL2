# read in Casal2 MCMC results
cas2_mcmc <- list()
for (c in 1:num_C2_models)
{
    cas2_mcmc_path <- file.path(base_dir, 'Casal2', C2_subdir[c])

    cas2_mcmc[[c]] <- casal2::extract.mcmc(C2_sample_file, C2_obj_fun_file, cas2_mcmc_path, return_covariance=TRUE)
}

