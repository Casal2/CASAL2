plot_legend <- function()
{
    # placeholder plot for legend
    plot(0, 0, type='n', xlim=c(0,1), ylim=c(0,1), axes=FALSE, xlab='', ylab='', main='')
    legend(0.1, 0.99, c('CASAL', 'CASAL sens1', C2_subdir), lwd=6, col=c('black', 'grey', C2_color), pt.cex=1.7)
}
