#' Utility extract function
#'
#' @author Dan Fu
#' @param quiet suppress print or cat statements to screen.
#' @keywords internal
#'
"convert.to.lines" <- function(filename, fileEncoding, quiet = quiet) {
  file <- scan(filename, what = "", sep = "\n", fileEncoding = fileEncoding, quiet = quiet)
  return(file)
}
