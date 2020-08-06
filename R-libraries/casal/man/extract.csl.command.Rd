\name{extract.csl.command}
\alias{extract.csl.command}
\title{Extracts the commands and sub-commands from a CASAL 'csl' input parameter file}
\description{
Creates an R object that is a subset of an R object of class 'casal.csl'
}
\usage{
extract.csl.command(object,command)
}
\arguments{
  \item{object}{An R object created by 'extract.csl.file', which is a list object containing the commands and subcommands from a CASAL input parameter file.}
  \item{command}{The command to extract from the object}
  \item{label}{Optionally, the command's label to extract}
  \item{where.subcommand}{Optionally, subcommand label and value to use to select the command to be extracted from the object}
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
