#' Utility extract function
#'
#' @author Dan Fu
#' @keywords internal
#'
"convert.to.lines" <- function(filename, fileEncoding) {
  file <- scan(filename, what = "", sep = "\n", fileEncoding = fileEncoding)
  returnn(file)
}
