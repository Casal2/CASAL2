#' Utility extract function
#'
#' @author C Marsh
#' @keywords internal
#'
make.data_frame_with_row_labs <- function(lines) {
  columns <- string.to.vector.of.words(lines[1])[-1] ## remove the first element it should be label of rows
  data <- data.frame(matrix(nrow = length(lines) - 1, ncol = length(columns)))
  row_labs = vector();
  for (i in 2:length(lines)) {
    line = string.to.vector.of.words(lines[i])
    row_labs = c(row_labs, line[1])
    line = line[-1]
    if (length(line) != length(columns)) {
      stop(paste(lines[i], "is not the same length as", lines[1]))
    }

    data[i - 1,] <- line
  }
  colnames(data) <- columns
  rownames(data) = row_labs;
  for (i in 1:ncol(data)) {
    if (is.all.numeric(data[, i])) {
      data[, i] = as.numeric(data[, i])
    }
  }
  data
}

