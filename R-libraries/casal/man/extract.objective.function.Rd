\name{extract.objective.function}
\alias{extract.objective.function}
\title{Extracts the objective function information from a CASAL standard output file}
\description{
Creates an R object representing objective function information contained in a CASAL standard output file.
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
  extract.objective.function returns a partial list of the contents of the file that describe the objective function information. This includes objective function value and components.
  \item{value}{Objective function value}
  \item{components}{data.frame of the objective function component labels and the component values}
}
\note{
The exact format of the object on the file is subject to change. No error checking is undertaken to ensure that the data in the R object accurately represents the data in the CASAL file. Note also that exact equality of read-in numeric data is machine specific.
}
\seealso{
\code{\link{casal-package}}
}
