\name{write.csl.file}
\alias{write.csl.file}
\title{Writes an R object (created with 'extract.csl.file') out to an external file, in a form suitable for use as a 'csl' input parameter file for CASAL (i.e., population.csl, estimation.csl, or output.csl}

\description{
Creates an external text file, with commands and sub-commands as for a CASAL input parameter file (i.e., population.csl, estimation.csl, or output.csl from an R object created with 'extract.csl.file'
}
\usage{
write.csl.file(object,file,path)
}
\arguments{
  \item{object}{An R object containing a list of the CASAL commands and subcommands (see \link{extract.csl.file})}
  \item{file}{character string giving the file name of the output file.}
  \item{path}{Optional character string giving the path to 'file'.}
}
\value{
  Invisible.
}
\note{
Illegal names can be used to name commands or subcommands in CASAL. Such names are converted to legal names by enclosing the string in quotes. The exact format of the object on the file is subject to change. No error checking is undertaken to ensure that the data in the R object accurately represents the data in the CASAL file. Note also that exact equality of read-in numeric data is machine specific.
}
\seealso{
\code{\link{casal-package}}
}
