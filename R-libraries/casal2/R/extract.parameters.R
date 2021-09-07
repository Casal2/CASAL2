#' Utility extract.parameters function
#'
#' This function reads in a parameter file that would be generated using the -o syntax.
#'
#' @author Craig Marsh
#' @param file The name of the input file containing model output to extract
#' @param path The path to the file (optional)
#' @param fileEncoding Switch to read in files that have been encoded in alternative UTF formats (optional); see the Casal2 User Manual for the error message that would indicate when to use this switch.
#' @param quiet suppress print or cat statements to screen.
#' @return Data <"data.frame"> of parameters that are from a -i format.
#' @export
#'
"extract.parameters" <- function(file, path = "", fileEncoding = "", quiet = FALSE) {
  filename <- make.filename(path = path, file = file)
  file <- convert.to.lines(filename, fileEncoding = fileEncoding, quiet = quiet)
  ## remove tabs from header
  file[1] <- gsub(pattern = "\t", replacement = " ", x = file[1], fixed = TRUE)
  Data <- make.data.frame(file)
  return(Data)
}
