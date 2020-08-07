\name{extract.mpd}
\alias{extract.mpd}
\title{Extracts header, objective function, free parameters, fits, and quantities from a CASAL standard output file}
\description{
Creates an R list object representing the information contained in a CASAL standard output file.
}
\usage{
extract.objective.function(file,path)
}
\arguments{
  \item{file}{character string giving the file name where the data is to be retrieved.}
  \item{path}{Optional character string giving the path to 'file'.}
}
\value{
  A list object.
  extract.mpd returns a partial list of the contents of a standard CASAL output file.
  \item{header}{an R object representing header file information contained in a CASAL standard output file}
  \item{objective.function}{an R object representing objective function information contained in a CASAL standard output file.}
  \item{bounds}{an R dataframe representing free parameters, value, and the lower and upper bounds contained in a CASAL standard output file.}
  \item{free}{an R object representing the observations, fits, and residual information contained in a CASAL standard output file.}
  \item{fits}{an R object representing free parameter information contained in a CASAL standard output file.}
  \item{quantities}{an R object representing the estimated quantities contained in a CASAL standard output file.}
}
\note{
The exact format of the object on the file is subject to change. No error checking is undertaken to ensure that the data in the R object accurately represents the data in the CASAL file. Note also that exact equality of read-in numeric data is machine specific.
}
\seealso{
\code{\link{casal-package}}
}
