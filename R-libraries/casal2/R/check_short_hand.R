#' Utility extract function
#'
#' @author Craig Marsh
#'

"check_short_hand" <- function(x) {
  string_ndx <- rep(F, length(x))
  numeric_ndx <- string_ndx
  for (i in 2:length(x)) {
    ## check for a string shorthand syntax
    string_ndx[i] <- grepl(",", x[i])
    string_ndx[i] <- grepl("\\*", x[i])
    ## check for a numeric shorthand syntax
    numeric_ndx[i] <- grepl(":", x[i])
  }
  return(list(string = string_ndx, numeric = numeric_ndx))
}
