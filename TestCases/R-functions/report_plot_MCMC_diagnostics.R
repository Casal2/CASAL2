
coda_mcmc_list <- list()

coda_mcmc_list[[1]] <- coda::mcmc(C1_mcmc_mat)
coda_mcmc_list[[2]] <- coda::mcmc(C1_mcmc_sens1_mat)

for (c in 1:num_C2_models)
{
    coda_mcmc_list[[(c+2)]] <- coda::mcmc(C2_mcmc_mat[[c]])
}


ggs_all_chains <- ggs(coda::mcmc.list(coda_mcmc_list))

ggmcmc(ggs_all_chains, file.path(base_dir, 'ggmcmc_comparison.pdf'))
