\name{extract.parameters.at.bounds}
\alias{extract.parameters.at.bounds}
\title{Extracts the list free parameters that are at or near a bound from a CASAL standard output file}
\description{
Creates an R dataframe representing free parameters, value, and the lower and upper bounds contained in a CASAL standard output file.
}
\usage{
extract.parameters.at.bounds(file,path)
}
\arguments{
  \item{file}{character string giving the file name where the data is to be retrieved.}
  \item{path}{Optional character string giving the path to 'file'.}
}
\value{
  A dataframe.
  extract.parameters.at.bounds returns a dataframe of the free parameters that are at or near a bound.
}
\note{
Illegal names can be used to name free parameters in CASAL. Such names are converted to legal names by enclosing the string in quotes. The exact format of the object on the file is subject to change. No error checking is undertaken to ensure that the data in the R object accurately represents the data in the CASAL file. Note also that exact equality of read-in numeric data is machine specific.
}
\seealso{
\code{\link{casal-package}}
}
