#' Utility extract.parameters function
#'
#' This function reads in a parameter file that would be generated using the -o syntax.
#'
#' @author Craig Marsh
#' @param file the name of the input file containing model output to extract
#' @param path Optionally, the path to the file
#' @param fileEncoding Optional, allows the R-library to read in files that have been encoded in alternative UTF formats, see the manual for the error message that would indicate when to use this switch.
#' @return Data <"data.frame"> of parameters that are from a -i format.
#' @export
#'
"extract.parameters" = function (file, path = "", fileEncoding = "") {
  filename = make.filename(path = path, file = file);
  file = convert.to.lines(filename, fileEncoding = fileEncoding);
  ## remove tabs from header
  file[1] = gsub(pattern = "\t", replace = " ",x = file[1], fixed = TRUE)
  Data = make.data.frame(file);
  return(Data);
}

