\name{extract.fits}
\alias{extract.fits}
\title{Extracts the observation and associated fit & residual information from a CASAL standard output file}
\description{
Creates an R object representing the observations, fits, and residual information contained in a CASAL standard output file.
}
\usage{
extract.fits(file,path)
}
\arguments{
  \item{file}{character string giving the file name where the data is to be retrieved.}
  \item{path}{Optional character string giving the path to 'file'.}
}
\value{
  A list object.
  extract.fits returns a partial list of the contents of the file that describe the observation, fit, and residual information. Each element of the object represents a single time series of observations from the CASAL input files. List elements are named by the text string used to define time series in CASAL.
  \item{year}{Vector of years that identify the years of each of the observations}
  \item{obs}{Vector or matrix of observation data}
  \item{fits}{Vector or matrix of associated fitted value for each observation}
  \item{resids}{Vector or matrix of associated residuals for each observation}
  \item{"pearson_resids"}{Vector or matrix of associated Pearson residuals for each observation}
  \item{"normalised_resids"}{Vector or matrix of associated normalised residuals for each observation}
}
\note{
Illegal names can be used to name free parameters in CASAL. Such names are converted to legal names by enclosing the string in quotes. The exact format of the object on the file is subject to change. No error checking is undertaken to ensure that the data in the R object accurately represents the data in the CASAL file. Note also that exact equality of read-in numeric data is machine specific.
}
\seealso{
\code{\link{casal-package}}
}
