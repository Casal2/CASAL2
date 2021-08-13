#' Utility function
#'
#' @author Craig Marsh
#' @keywords internal
#'
"get.casal2.unique_subcommands_list" <- function() {
  command <- c("table", "end_table")
  type <- c("table_label", "end_table")
  casal2_list <- list(command = command, type = type)
  return(casal2_list)
}
