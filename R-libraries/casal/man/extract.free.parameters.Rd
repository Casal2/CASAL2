\name{extract.free.parameters}
\alias{extract.free.parameters}
\title{Extracts free parameters information from a CASAL standard output file}
\description{
Creates an R object representing free parameter information contained in a CASAL standard output file.
}
\usage{
extract.free.parameters(file,path,type=c("values","bounds","both"))
}
\arguments{
  \item{file}{character string giving the file name where the data is to be retrieved.}
  \item{path}{Optional character string giving the path to 'file'.}
  \item{type}{Items to output. Either the free parameters, the bounds, or both.}
}
\value{
  A list object.
  extract.free.parameters returns a partial list of the contents of the file that describe the free parameter information. This would typically include the initialization.B0 value, selectivity estimates, and year class strength estimates. List elements are named by the text string used to define each free parameter in CASAL.
  If type='both', then a list object with two elements is returned;
  \item{values}{list object giving the values of the free parameters.}
  \item{bounds}{list object giving the upper and lower bounds of the free parameters.}
}
\note{
CASAL does not output parameter bounds in version before v2.10-2007/12/05. For output from older versions of CASAL, extract.free.parameters will return NAs for the bounds argument.

Illegal names can be used to name free parameters in CASAL. Such names are converted to legal names by enclosing the string in quotes. The exact format of the object on the file is subject to change. No error checking is undertaken to ensure that the data in the R object accurately represents the data in the CASAL file. Note also that exact equality of read-in numeric data is machine specific.
}
\seealso{
\code{\link{casal-package}}
}
