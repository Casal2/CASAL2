\name{extract.csl.file}
\alias{extract.csl.file}
\title{Extracts the commands and sub-commands from a CASAL 'csl' input parameter file}
\description{
Creates an R object representing the commands and sub-commands from a CASAL input parameter file (i.e., population.csl, estimation.csl, or output.csl
}
\usage{
extract.csl.file(file,path)
}
\arguments{
  \item{file}{character string giving the file name of the input parameter file.}
  \item{path}{Optional character string giving the path to 'file'.}
}
\value{
  A list object that is a list of lists. i.e., a list object where each CASAL command is an element of the list, and the each of these has elements made up of (a) the command, (b) its value, and (c) its subcommands and their values.
  Note that each element of the list is labelled;

    'command[label]' for those commands with labels

    'command[number]' where \emph{number} is a unique number, for auto-numbered commands

    'command' for those commands with arguments or where no label is supplied for optionally labelled commands

  Each of these elements is a list object, with member elements;

    'command' the CASAL command (without any labels)

    'value' the label or arguments to the command

    and then elements labelled for each subcommand, each with values equal to the subcommand arguments.
}
\note{
Illegal names can be used to name commands or subcommands in CASAL. Such names are converted to legal names by enclosing the string in quotes. The exact format of the object on the file is subject to change. No error checking is undertaken to ensure that the data in the R object accurately represents the data in the CASAL file. Note also that exact equality of read-in numeric data is machine specific.
}
\seealso{
\code{\link{casal-package}}
}
