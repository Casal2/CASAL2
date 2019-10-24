\name{extract.mpd.from.file}
\alias{extract.mpd.from.file}
\title{Extracts the point estimate (MPD) from a from a CASAL standard output file (i.e., the data following the line "In a format suitable for -i")}
\description{
Creates an R object representing free parameter information contained in a CASAL standard output file.
}
\usage{
extract.mpd.from.file(file,path)
}
\arguments{
  \item{file}{character string giving the file name where the data is to be retrieved.}
  \item{path}{Optional character string giving the path to 'file'.}
}
\value{
  A string 2-vector containing [1] a string of the parameter names and [2] a string of the parameter values.
}
\note{
Illegal names can be used to name free parameters in CASAL. Such names are converted to legal names by enclosing the string in quotes. The exact format of the object on the file is subject to change. No error checking is undertaken to ensure that the data in the R object accurately represents the data in the CASAL file. Note also that exact equality of read-in numeric data is machine specific.
}
\seealso{
\code{\link{casal-package}}
}
