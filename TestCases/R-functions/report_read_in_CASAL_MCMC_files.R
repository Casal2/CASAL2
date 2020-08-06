# read in CASAL MCMC results
cas_mcmc_path <- file.path(base_dir, 'CASAL')
cas_mcmc <- casal::extract.mcmc(C1_sample_file, C1_obj_fun_file, cas_mcmc_path)

cas_mcmc_sens1_path <- file.path(base_dir, 'CASAL_sens1')
cas_mcmc_sens1 <- casal::extract.mcmc(C1_sample_file, C1_obj_fun_file, cas_mcmc_sens1_path)
