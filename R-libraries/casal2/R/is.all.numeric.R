#' Utility extract function
#'
#' @author Dan Fu (not really)
#' @keywords internal
#'
"is.all.numeric" <-
function(x, what = c("test", "vector"), extras = c(".", "NA", "na", "null", "NULL")) {
  what <- match.arg(what)
  x <- sub("[[:space:]]+$", "", x)
  x <- sub("^[[:space:]]+", "", x)
  xs <- x[!x %in% c("", extras)]
  isnum <- suppressWarnings(!any(is.na(as.numeric(xs))))
  if (what == "test")
    isnum
  else if (isnum)
    as.numeric(x)
  else x
}
