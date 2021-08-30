#' Utility extract function
#'
#' @author Dan Fu
#' @keywords internal
#'

# 2021-08-30
# {d} -> {dataframe}
# {d_r} -> {dataframe_with_row_labels}
# {m} -> {matrix}
# {L} -> {list}
# end {L} -> {end_list}
# {v} -> {vector}
# {s} -> {string}
# {c} and {C} not used

"get.line.type" <-
function(line) {
  type = ""
  if (line == "*end")
    type = "*end"
  else if (line == "{end_list}")
    type = "{end_list}"
  else {
    line = string.to.vector.of.words(line)
    if (length(line) == 2 && substr(line[2], 1, 1) == "{" && substr(line[2], nchar(line[2]), nchar(line[2])) == "}") {
      type = substr(line[2], 2, nchar(line[2]) - 1)
      if (type != "dataframe" && type != "vector" && type != "list" && type != "matrix" && type != "string" && type != "dataframe_with_row_labels")
        stop("type from {} must be one of 'list', 'vector', 'dataframe', 'matrix', 'string', 'dataframe_with_row_labels'")
    } else if (substr(line[1], nchar(line[1]), nchar(line[1])) == ":")
      type = "L_E"
  }
  type
}
