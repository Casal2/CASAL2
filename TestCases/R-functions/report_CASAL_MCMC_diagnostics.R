# parameters C1_MCMC_params and num_samples_to_use

num_rows <- nrow(cas_mcmc)
if (num_rows < num_samples_to_use) {
    stop('ERROR: the number of CASAL base run samples is smaller than the number of requested samples')
}

num_rows_sens1 <- nrow(cas_mcmc_sens1)
if (num_rows < num_samples_to_use) {
    stop('ERROR: the number of CASAL sensitivity run samples is smaller than the number of requested samples')
}

C1_mcmc_mat       <- cas_mcmc[((num_rows - num_samples_to_use + 1):num_rows), C1_MCMC_params]
colnames(C1_mcmc_mat) <- MCMC_params_rename

C1_mcmc_sens1_mat <- cas_mcmc_sens1[((num_rows_sens1 - num_samples_to_use + 1):num_rows), C1_MCMC_params]
colnames(C1_mcmc_sens1_mat) <- MCMC_params_rename
