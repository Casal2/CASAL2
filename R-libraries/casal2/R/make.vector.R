#' Utility extract function
#'
#' @author Dan Fu
#' @keywords internal
#'
"make.vector" <-
function(lines) {
  n = length(lines)
  data = vector("numeric", length = n)
  names = vector("character", length = n)
  for (i in 1:n) {
    line = string.to.vector.of.words(lines[i])
    if (length(line) != 2) {
      stop(paste("Expecting a vector of length 2 from line:", lines[i]))
    }
    names[i] = line[1]
    data[i] = as.numeric(line[2])
  }
  names(data) = names
  data
}

