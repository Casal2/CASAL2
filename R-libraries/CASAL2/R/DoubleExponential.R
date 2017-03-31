#' Calculate the Double-Exponential Selectivity used in Casal2
#'
#' This function calculates the Double-Exponential selectivity and should be used to plot up the form of the selectivity when estimating the parameters in the model.
#'
#' @author Craig Marsh
#' @param age_or_length "vector<numeric>" A vector of ages and or lengths to calculate the selectivity over.
#' @param x0 "numeric" See manual for formal definition of these parameters
#' @param x1 "numeric" 
#' @param x2 "numeric" 
#' @param y0 "numeric" 
#' @param y1 "numeric" 
#' @param y2 "numeric" 
#' @param alpha "numeric" The capping parameter of the selectivity, to move the max away from one
#' @return "vector<numeric>" selectivity values over the age/length range and parameters supplied
#' @export
#'
DoubleExponential = function(age_or_length,x0,x1,x2,y0,y1,y2,alpha = 1) {
 selectivity = vector();
 for(i in 1:length(age_or_length)) {
  if(age_or_length[i] <= x0) {
    selectivity[i] = alpha * y0 * pow((y1 / y0), (age_or_length[i] - x0)/(x1 - x0));
  } else if (age_or_length[i] > x0 & age_or_length[i] <= x2) {
   selectivity[i] = alpha * y0 * pow((y2 / y0), (age_or_length[i] - x0)/(x2 - x0));
  } else {
   selectivity[i] = y2;
  }
 }
 return(selectivity);
}