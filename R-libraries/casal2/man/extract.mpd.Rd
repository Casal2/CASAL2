% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/extract.mpd.R
\name{extract.mpd}
\alias{extract.mpd}
\title{extract MPD function for readin in Casal2 output that has been generated from a -r, -e, -f, -p run mode.}
\usage{
extract.mpd(file, path = "", fileEncoding = "", quiet = FALSE)
}
\arguments{
\item{file}{the name of the input file containing model output to extract}

\item{path}{Optionally, the path to the file}

\item{fileEncoding}{Optional, allows the R-library to read in files that have been encoded in alternative UTF formats, see the manual for the error message that would indicate when to use this switch.}

\item{quiet}{suppress print or cat statements to screen.}
}
\value{
a 'casal2MPD' object which is essentially a list, that can be integrated using the str() function.
}
\description{
An extract function that reads Casal2 output that are produced from a '-r' or '-e' or '-f' or '-p' model run. This function
also creates a 'casal2.mpd' class which can be used in plotting and summary functions. See the casal2 manual for more information.
}
\examples{
\donttest{
library(casal2)
data <- extract.mpd(file = system.file("extdata", "MPD.log", package = "casal2"))
class(data)
}
}
\author{
Dan Fu
}
