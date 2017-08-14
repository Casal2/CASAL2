#' Utility extract.parameters function
#'
#' This function reads in a parameter file that would be generated using the -o syntax.
#'
#' @author Craig Marsh
#' @param file the name of the input file containing model output to extract
#' @param path Optionally, the path to the file
#' @return Data <"data.frame"> of parameters that are from a -i format.
#' @export
#'
"extract.parameters" = function (file, path = "") {
  filename = make.filename(path = path, file = file);
  file = convert.to.lines(filename);
  ## remove tabs from header
  file[1] = gsub(pattern = "\t", replace = " ",x = file[1], fixed = TRUE)
  Data = make.data.frame(file);
  return(Data);
}

