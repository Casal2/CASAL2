\name{extract.mulitiple.objective.functions}
\alias{extract.mulitiple.objective.functions}
\title{Extracts mulitiple objective functions from a CASAL standard output file}
\description{
Creates an matrix with rows representing the objective function information and columns for each parameter set contained in a CASAL standard output file.
}
\usage{
extract.multiple.objective.functions(file,path)
}
\arguments{
  \item{file}{character string giving the file name where the data is to be retrieved.}
  \item{path}{Optional character string giving the path to 'file'.}
}
\value{
  A matrix.
  extract.multiple.objective.functions returns a matrix of the individual objective function components as rows, and parameter sets as columns.
}
\note{
Illegal names can be used to name free parameters in CASAL. Such names are converted to legal names by enclosing the string in quotes. The exact format of the object on the file is subject to change. No error checking is undertaken to ensure that the data in the R object accurately represents the data in the CASAL file. Note also that exact equality of read-in numeric data is machine specific.
}
\seealso{
\code{\link{casal-package}}
}
