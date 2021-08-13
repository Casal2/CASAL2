#' @title burn.in.tabular used to return a list that has had the beginning cut off.
#'
#' @description
#' returns a casal2TAB class that has been shortened to the ith iteration
#'
#' @author Craig Marsh
#' @param tab_object casal2TAB object you want to burn-in
#' @param Row number to burn in from, note this is not the iteration but the row that corresponds to your iteration that you want to burn-in from. if keep > 1 then the iteration and row will be different
#' @return a 'casal2TAB' object which has been manipulated.
#' @export
#'
"burn.in.tabular" <- function(tab_object, Row) {
  if (class(tab_object) != "casal2TAB") {
    stop(paste0("The tab_object must be class 'casal2TAB', but the parsed object is class '", class(tab_object), "'"))
  }
  for (i in 1:length(names(tab_object))) {
    this_list <- get(names(tab_object)[i], tab_object)
    if (Row >= nrow(this_list$values)) {
      stop(paste0("Row = ", Row, ", which is larger than the number of rows in the casl2TAB object('", nrow(this_list$values), "). The Row value is the ith sample after Casal2 has thinned the sample"))
    }
    this_list$values <- this_list$values[Row:nrow(this_list$values),]
    print(paste0("rows remaining after burn-in = ", nrow(this_list$values)))
    ## try and store it back in to the original object.
    tab_object[[i]]$values <- this_list$values
  }
  return(tab_object)
}
