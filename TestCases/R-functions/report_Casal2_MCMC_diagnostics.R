# parameters C2_MCMC_params and num_samples_to_use

# > names(cas2_mcmc[[1]])
#  [1] "Data"       "Covariance"

C2_mcmc_mat     <- list()
for (c in 1:num_C2_models)
{
    num_rows <- nrow(cas2_mcmc[[c]]$Data)
    if (num_rows < num_samples_to_use) {
        stop(paste('ERROR: the number of Casal2', C2_subdir[c], 'run samples is smaller than the number of requested samples'))
    }

    C2_mcmc_mat[[c]] <- cas2_mcmc[[c]]$Data[seq((num_rows - num_samples_to_use + 1), num_rows, num_to_subsample), C2_MCMC_params]
    colnames(C2_mcmc_mat[[c]]) <- MCMC_params_rename
}
