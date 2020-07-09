plot_image_and_range <- function(corr_obj, label)
{
    image(corr_obj, zlim=c(-1,1), col=hcl.colors(51, 'RdYlBu'), xlab='Parameter', ylab='Parameter', main=label)

    print(paste(label, 'range (excluding 1.0):', paste0(range(corr_obj[corr_obj < 1], finite=TRUE), collapse=' ')))
}

plot_image_and_range(cas_corr, 'CASAL base parameter correlation')

plot_image_and_range(cas_sens1_corr, 'CASAL sensitivity 1 parameter correlation')

for (c in 1:num_C2_models)
{
    plot_image_and_range(cas2_corr[[c]], paste('Casal2', C2_subdir[c], 'parameter correlation'))
}

