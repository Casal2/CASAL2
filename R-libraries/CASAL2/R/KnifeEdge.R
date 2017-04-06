#' Calculate the KnifeEdge Selectivity used in Casal2
#'
#' This function calculates the KnifeEdge selectivity and should be used to plot up the form of the selectivity when estimating the parameters in the model.
#'
#' @author Craig Marsh
#' @param age_or_length "vector<numeric>" A vector of ages and or lengths to calculate the selectivity over.
#' @param edge "numeric" age or length at which selectivity is 0 to the left or alpha to the right
#' @param alpha "numeric" The capping parameter of the selectivity, to move the max away from one
#' @return "vector<numeric>" selectivity values over the age/length range and parameters supplied
#' @export
#'
KnifeEdge = function(age_or_length, alpha = 1, Edge) {
 selectivity = vector();
 for (i in 1:length(age_or_length)) {
  if (age_or_length[i] >= Edge) {
   selectivity[i] = alpha;
  } else {
   selectivity[i] = 0.0;
  }
 }
 return(selectivity);
}