\name{extract.header}
\alias{extract.header}
\title{Extract the header file information from a CASAL standard output file.}
\description{
Creates an R object representing header file information contained in a CASAL standard output file.
}
\usage{
extract.header(file,path)
}
\arguments{
  \item{file}{character string giving the file name where the data is to be retrieved.}
  \item{path}{Optional character string giving the path to 'file'.}
}
\value{
  A list object.
  extract.header returns a partial list of the contents of the file that describe the header information. This includes CASAL function call, run date and time, CASAL version information, user and machine names.
  \item{call}{CASAL function call and command line parameters}
  \item{date}{Creation date time of the output file}
  \item{version}{Version identification information}
  \item{user}{User (login) name}
  \item{machine}{Machine (computer) name}
}
\note{
The exact format of the object on the file is subject to change. No error checking is undertaken to ensure that the data in the R object accurately represents the data in the CASAL file. Note also that exact equality of read-in numeric data is machine specific.
}
\seealso{
\code{\link{casal-package}}
}
