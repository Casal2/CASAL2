\name{extract.mcmc}
\alias{extract.mcmc}
\title{Extracts the output samples or quantities from a CASAL MCMC output file into a dataframe object}
\description{
Creates an R object representing the samples or output quantities contained in the CASAL MCMC output files.
}
\usage{
extract.mcmc(samples,objectives,burn.in = 0)
}
\arguments{
  \item{samples}{character string giving the file name where either (i) the MCMC chain data is to be retrieved (the samples.* file), or (ii) the output quantities file (as generated with casal -v).}
  \item{objectives}{optional character string giving the file name where the objective function data is to be retrieved.}
  \item{burn.in}{number specifying the number of samples at the beginning of the samples.file that should be considered to be samples from the “burn-in” phase of the MCMC run.}
}
\value{
  A data.frame object, with one row per output quantity.
}
\note{
Illegal names can be used to name free parameters in CASAL. Such names are converted to legal names by enclosing the string in quotes. The exact format of the object on the file is subject to change. No error checking is undertaken to ensure that the data in the R object accurately represents the data in the CASAL file. Note also that exact equality of read-in numeric data is machine specific.
}
\seealso{
\code{\link{casal-package}}
}
