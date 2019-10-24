"minor.tick"<-
function (nx = 2, ny = 2, tick.ratio = 0.5)
{
# From
#Package: Hmisc
#Version: 2.0-9
#Date: 2004-03-22
#Title: Harrell Miscellaneous
#Author: Frank E Harrell Jr <f.harrell@vanderbilt.edu>, with contributions from many other users.
#Maintainer: Frank E Harrell Jr <f.harrell@vanderbilt.edu>
#Depends: R (>= 1.8), grid, lattice, acepack
#Description: The Hmisc library contains many functions useful for data analysis, high-level graphics, utility
#            operations, functions for computing sample size and power, importing datasets, imputing missing
#            values, advanced table making, variable clustering, character string manipulation, conversion of S
#            objects to LaTeX code, and recoding variables.
#License: GPL version 2 or newer
    ax <- function(w, n, tick.ratio) {
        range <- par("usr")[if (w == "x") 1:2 else 3:4]
        tick.pos <- if (w == "x")
          par("xaxp")
        else
          par("yaxp")
        distance.between.minor <- (tick.pos[2] - tick.pos[1])/tick.pos[3]/n
        possible.minors <- tick.pos[1] - (0:100) * distance.between.minor
        if(length(possible.minors[possible.minors >= range[1]])>0)
          low.minor <- min(possible.minors[possible.minors >= range[1]])
        else
          low.minor<-NA
        if (is.na(low.minor)) low.minor <- tick.pos[1]
        possible.minors <- tick.pos[2] + (0:100) * distance.between.minor
        if(length(possible.minors[possible.minors <= range[2]])>0)
          hi.minor <- max(possible.minors[possible.minors <= range[2]])
        else
          hi.minor<-NA
        if (is.na(hi.minor)) hi.minor <- tick.pos[2]
        axis(if (w == "x") 1 else 2, seq(low.minor, hi.minor, by = distance.between.minor), labels = FALSE, tcl = par("tcl") * tick.ratio)
    }
    if (nx > 1) ax("x", nx, tick.ratio = tick.ratio)
    if (ny > 1) ax("y", ny, tick.ratio = tick.ratio)
    invisible()
}
