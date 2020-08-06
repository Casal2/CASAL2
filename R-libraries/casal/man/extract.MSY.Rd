\name{extract.MSY}
\alias{extract.MSY}
\title{Extracts MSY output from a CASAL yields run (casal -Y) when doing determinsitic yields}
\description{
Creates an R list object representing the information contained in the yield output file.
}
\usage{
extract.MSY(file,path)
}
\arguments{
  \item{file}{character string giving the file name where the data is to be retrieved.}
  \item{path}{Optional character string giving the path to 'file'.}
}
\value{
  A list object.
  extract.mpd returns a partial list of the contents of a standard CASAL output file.
  \item{MSY}{an R object representing results of an MSY yield estimation in CASAL.}
}
\note{
The exact format of the object on the file is subject to change. No error checking is undertaken to ensure that the data in the R object accurately represents the data in the CASAL file. Note also that exact equality of read-in numeric data is machine specific.
}
\seealso{
\code{\link{casal-package}}
}
