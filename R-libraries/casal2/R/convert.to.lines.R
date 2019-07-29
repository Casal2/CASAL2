#' Utility extract function
#'
#' @author Dan Fu
#' @keywords internal
#'
"convert.to.lines" <-
function(filename, fileEncoding) {
  scan(filename, what = "", sep = "\n", fileEncoding = fileEncoding)
}

