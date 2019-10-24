\name{extract.parameter.bounds}
\alias{extract.parameter.bounds}
\title{Extracts parameter bounds for all estimated parameters in an estimation.csl file}
\description{
Extracts parameter bounds for all estimated parameters in an estimation.csl file, returning these either as an n x 2 matrix (if output.type='matrix') or a list containing one component for each parameter (or parameter vector), where each component is a list with components 'lowerbound' and 'upperbound'.
}
\usage{
extract.parameter.bounds(file,path)
}
\arguments{
  \item{file}{character string giving the file name where the data is to be retrieved.}
  \item{path}{Optional character string giving the path to 'file'.}
  \item{output.type}{Specify either a matrix or a list (default=matrix).}
}
\value{
  An n x 2 matrix or list object, with components
  \item{lowerbound}{The parameter lower bound}
  \item{lowerbound}{The parameter upper bound}
}
\note{
Illegal names can be used to name free parameters in CASAL. Such names are converted to legal names by enclosing the string in quotes. The exact format of the object on the file is subject to change. No error checking is undertaken to ensure that the data in the R object accurately represents the data in the CASAL file. Note also that exact equality of read-in numeric data is machine specific.
}
\seealso{
\code{\link{casal-package}}
}
