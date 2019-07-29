pkgname <- "casal2"
source(file.path(R.home("share"), "R", "examples-header.R"))
options(warn = 1)
library('casal2')

base::assign(".oldSearch", base::search(), pos = 'CheckExEnv')
base::assign(".old_wd", base::getwd(), pos = 'CheckExEnv')
cleanEx()
nameEx("burn.in.tabular")
### * burn.in.tabular

flush(stderr()); flush(stdout())

### Name: burn.in.tabular
### Title: burn.in.tabular used to return a list that has had the beginning
###   cut off.
### Aliases: burn.in.tabular

### ** Examples

library(casal2)
# plotting Standard Output
tab <- extract.tabular(file = system.file("extdata", "tabular_report.out", package="casal2"))
burn_in_tab = burn.in.tabular(tab_object = tab, Row = 5);




cleanEx()
nameEx("extract.mpd")
### * extract.mpd

flush(stderr()); flush(stdout())

### Name: extract.mpd
### Title: extract MPD function for readin in Casal2 output that has been
###   generated from a -r, -e, -f, -p run mode.
### Aliases: extract.mpd

### ** Examples

library(casal2)
data <- extract.mpd(file = system.file("extdata", "MPD.log", package="casal2"))
class(data)



cleanEx()
nameEx("plot.fits")
### * plot.fits

flush(stderr()); flush(stdout())

### Name: plot.fits
### Title: plot.fits default
### Aliases: plot.fits plot.fits.casal2MPD plot.fits.casal2TAB

### ** Examples

library(casal2)
# plotting Standard Output
data <- extract.mpd(file = system.file("extdata", "estimate.log", package="casal2"))
names(data)
par(mfrow = c(1,2))
plot.fits(model = data, report_label = "Tangaroa_propn_at_age_Aug")
plot.fits(model = data, report_label = "wcsiTRLcpue")
# if you are unhappy with the default plotting you can use plot.it = FALSE and create a plot of your own.
Tangaroa_fits = plot.fits(model = data, report_label = "Tangaroa_propn_at_age_Aug", plot.it = FALSE)
# plotting Tabular Output
tab <- extract.tabular(file = system.file("extdata", "tabular_report.out", package="casal2"))
names(tab)
plot.fits(model = tab, report_label = "Tangaroa_propn_at_age_Aug")
par(mfrow = c(1,1))
plot.fits(model = tab, report_label = "wcsiTRLcpue")



graphics::par(get("par.postscript", pos = 'CheckExEnv'))
### * <FOOTER>
###
cleanEx()
options(digits = 7L)
base::cat("Time elapsed: ", proc.time() - base::get("ptime", pos = 'CheckExEnv'),"\n")
grDevices::dev.off()
###
### Local variables: ***
### mode: outline-minor ***
### outline-regexp: "\\(> \\)?### [*]+" ***
### End: ***
quit('no')
