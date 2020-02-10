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
#' @examples
#' library(casal2)
#' # plotting Standard Output
#' tab <- extract.tabular(file = system.file("extdata", "tabular.out", package="casal2"))
#' burn_in_tab = burn.in.tabular(tab_object = tab, Row = 5);
#'
"burn.in.tabular" <-
function (tab_object, Row) {
  if (class(tab_object) != "casal2TAB") {
    stop(Paste("The tab_object must be class 'casal2TAB', but the parsed object is class '", class(tab_object), "'"))
  }

  for (i in 1:length(names(tab_object))) {
    this_list = get(names(tab_object)[i], tab_object)
    if (Row >= nrow(this_list$values)) {
      stop(Paste("Row = ", Row, ", which is larger than the number of rows in the casl2TAB object('", nrow(this_list$values), "). The Row value is the ith sample after Casal2 has thinned the sample"))
    }
    this_list$values = this_list$values[Row:nrow(this_list$values),]
    print(Paste("rows remaining after burn-in = " ,nrow(this_list$values)))
    ## try and store it back in to the original object.
    tab_object[[i]]$values = this_list$values
  }
  return(tab_object);
}

