\name{extract.quantities}
\alias{extract.quantities}
\title{Extracts the estimated quantities from a CASAL standard output file}
\description{
Creates an R object representing the estimated quantities contained in a CASAL standard output file.
}
\usage{
extract.quantities(file,path)
}
\arguments{
  \item{file}{character string giving the file name where the data is to be retrieved.}
  \item{path}{Optional character string giving the path to 'file'.}
}
\value{
  A list object.
  extract.quantities returns a partial list of the contents of the file that describe the estimated free quantities. This would typically include scalar and vector parameter values, ogives, q’s, B0, R0, and spawning stock biomass estimates. Each element of the object represents a single quantity from the CASAL output files. These elements can include, but are not limited to, the following
  \item{"Scalar parameter values"}{List object describing the values of each of the scalar parameter values.}
  \item{"Vector parameter values"}{List object describing the values of each of the vector parameter values.}
  \item{"Vector ogive arguments"}{List object describing the values of each of the vector ogive values.}
  \item{"Nuisance q's"}{List object describing the values of each of the nuisance q values.}
  \item{B0}{Estimated value of B0}
  \item{R0}{Estimated value of R0}
  \item{SSBs}{List object with vector elements}
  \item{SSBs$SSB}{Vector of spawning stock biomass estimates}
  \item{SSBs$year}{Vector of years associated with the spawning stock biomass estimates}
  \item{"Actual_catches"$catch}{Vector of recorded catch values}
  \item{"Actual_catches"$year}{Vector of years associated with the catch}
}
\note{
Illegal names can be used to name free parameters in CASAL. Such names are converted to legal names by enclosing the string in quotes. The exact format of the object on the file is subject to change. No error checking is undertaken to ensure that the data in the R object accurately represents the data in the CASAL file. Note also that exact equality of read-in numeric data is machine specific.
}
\seealso{
\code{\link{casal-package}}
}
