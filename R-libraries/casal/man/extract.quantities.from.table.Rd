\name{extract.quantities.from.table}
\alias{extract.quantities.from.table}
\title{Extracts the output quantities from a CASAL output file in tabular format.}
\description{
Creates an R object representing the output quantities contained in a CASAL tabular format file.}
\usage{
extract.quantities.from.table(file,path)
}
\arguments{
  \item{file}{character string giving the file name where the data is to be retrieved.}
  \item{path}{Optional character string giving the path to 'file'.}
}
\value{
  A data.frame object, with one column per free parameter.
}
\note{
Illegal names can be used to name free parameters in CASAL. Such names are converted to legal names by enclosing the string in quotes. The exact format of the object on the file is subject to change. No error checking is undertaken to ensure that the data in the R object accurately represents the data in the CASAL file. Note also that exact equality of read-in numeric data is machine specific.
}
\seealso{
\code{\link{casal-package}}
}
