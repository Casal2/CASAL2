#' Utility extract function
#'
#' @author Dan Fu
#' @keywords internal
#'
remove.last.words <- function(string, words = 1) {
  temp <- unpaste(string, sep = " ")
  to.drop <- length(temp) - (0:(words - 1))
  paste(unlist(temp[-to.drop]), collapse = " ")
}

