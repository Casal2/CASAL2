
coda_mcmc_list <- list()

coda_mcmc_list[[1]] <- coda::mcmc(C1_mcmc_mat)
coda_mcmc_list[[2]] <- coda::mcmc(C1_mcmc_sens1_mat)

for (c in 1:num_C2_models)
{
    coda_mcmc_list[[(c+2)]] <- coda::mcmc(C2_mcmc_mat[[c]])
}

# coda::acfplot(coda_mcmc_list[[1]])
# coda::autocorr.diag(coda_mcmc_list[[1]])
# coda::autocorr.plot(coda_mcmc_list[[1]])
# coda::densplot(coda_mcmc_list[[1]])
# coda::HPDinterval(coda_mcmc_list[[1]])
# coda::heidel.diag(coda_mcmc_list[[1]])
# coda::raftery.diag(coda_mcmc_list[[1]])

# coda::cumuplot(coda_mcmc_list) [NOTE: interactive]
# coda::gelman.diag(coda_mcmc_list)
# coda::gelman.plot(coda_mcmc_list)
# coda::geweke.diag(coda_mcmc_list)
# coda::geweke.plot(coda_mcmc_list) [NOTE: interactive]
# coda::traceplot(coda_mcmc_list, col=c('black', 'grey', C2_color), lwd=c(2, 2, rep(1, 8)))


# coda::gelman.plot(coda_mcmc_list, ask=FALSE, col=c('black', 'grey', C2_color), lwd=c(2, 2, rep(1, 8)))
coda::gelman.diag(coda_mcmc_list)


lag_max <- 40

print(coda::acfplot(coda_mcmc_list[[1]], lag.max=lag_max, main='CASAL base model MCMC ACF'))
print(coda::acfplot(coda_mcmc_list[[2]], lag.max=lag_max, main='CASAL sensitivity 1 model MCMC ACF'))

for (c in 1:num_C2_models)
{
    print(coda::acfplot(coda_mcmc_list[[(c+2)]], lag.max=lag_max, main=paste('Casal2', C2_subdir[c], 'model MCMC ACF')))
}


# coda::cumuplot(coda_mcmc_list, ask=FALSE, col=c('black', 'grey', C2_color), lwd=c(2, 2, rep(1, 8)))

