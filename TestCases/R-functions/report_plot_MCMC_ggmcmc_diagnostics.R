plot_ggmcmc <- function(chain_list, filename, title='')
{
    # "by default, ggmcmc() plots ggs_histogram(), ggs_density(), ggs_traceplot(), ggs_running(),
    # ggs_compare_partial(), ggs_autocorrelation(), ggs_crosscorrelation(), ggs_Rhat(), ggs_effective(),
    # ggs_geweke() and ggs_caterpillar()"

    # ggmcmc(chain_list, filename, param_page=6, width=12, height=10)

    # p <- ggs_histogram(chain_list)
    # p <- p + labs(title = title)
    # plot(p)

    p <- ggs_density(chain_list)
    p <- p + labs(title = title)
    plot(p)

    p <- ggs_traceplot(chain_list)
    p <- p + labs(title = title)
    plot(p)

    p <- ggs_running(chain_list)
    p <- p + labs(title = title)
    plot(p)

    p <- ggs_compare_partial(chain_list)
    p <- p + labs(title = title)
    plot(p)

    p <- ggs_autocorrelation(chain_list)
    p <- p + labs(title = title)
    plot(p)

    # p <- ggs_crosscorrelation(chain_list)
    # p <- p + labs(title = title)
    # plot(p)

    # p <- ggs_Rhat(chain_list)
    # p <- p + labs(title = title)
    # plot(p)

    # p <- ggs_effective(chain_list)
    # p <- p + labs(title = title)
    # plot(p)

    p <- ggs_geweke(chain_list)
    p <- p + labs(title = title)
    plot(p)
}

ggs_all_chains <- ggs(coda::mcmc.list(coda_mcmc_list))
plot_ggmcmc(ggs_all_chains, file.path(base_dir, paste(plot_file_label, '.ggmcmc_comparison.ALL.html', sep='')), 'CASAL (chains 1 and 2) and Casal2 MCMC comparison')


ggs_C1_chains <- ggs(coda::mcmc.list(coda_mcmc_list[1:2]))
plot_ggmcmc(ggs_C1_chains, file.path(base_dir, paste(plot_file_label, '.ggmcmc_comparison.C1.html', sep='')), 'CASAL MCMC comparison')

ggs_C2_chains <- ggs(coda::mcmc.list(coda_mcmc_list[3:(num_C2_models+2)]))
plot_ggmcmc(ggs_C2_chains, file.path(base_dir, paste(plot_file_label, '.ggmcmc_comparison.C2.html', sep='')), 'Casal2 MCMC comparison')
