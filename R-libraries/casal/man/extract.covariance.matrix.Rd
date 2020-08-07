\name{extract.covariance.matrix}
\alias{extract.covariance.matrix}
\title{Extracts thje covariance matrix information from a CASAL standard output file}
\description{
Creates an R object (matrix) representing covariance matrix information contained in a CASAL standard output file.
}
\usage{
extract.covariance.matrix(file,path)
}
\arguments{
  \item{file}{character string giving the file name where the data is to be retrieved.}
  \item{path}{Optional character string giving the path to 'file'.}
}
\value{
  A list object.
  extract.covariance.matrix returns a matrix of the covariance matrix in the contents of the file that describe the free parameter information. This would typically include the initialization.B0 value, selectivity estimates, and year class strength estimates. 
}
\note{
Illegal names can be used to name free parameters in CASAL. Such names are converted to legal names by enclosing the string in quotes. The exact format of the object on the file is subject to change. No error checking is undertaken to ensure that the data in the R object accurately represents the data in the CASAL file. Note also that exact equality of read-in numeric data is machine specific.
}
\seealso{
\code{\link{casal-package}}
}
