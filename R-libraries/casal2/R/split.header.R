#' Utility extract function
#'
#' @author Craig Marsh
#' @keywords internal
#'
"split.header" <-
function (line) {
    start_index = as.numeric(regexpr(pattern = "\\[",text =line))
    stop_index = as.numeric(regexpr(pattern = "\\]",text = line))
    type = substr(line, 2, start_index - 1)
    label = substr(line, start_index + 1, stop_index - 1)
    return(c(label,type))
}