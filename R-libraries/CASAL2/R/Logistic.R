#' Calculate the Logistic Selectivity used in Casal2
#'
#' This function calculates the Logistic selectivity and should be used to plot up the form of the selectivity when estimating the parameters in the model.
#'
#' @author Craig Marsh
#' @param age_or_length "vector<numeric>" A vector of ages and or lengths to calculate the selectivity over.
#' @param a50 "numeric" age or length where selectivity 50\%
#' @param ato95 "numeric" age or length difference where selectivity goes from 50\% - 95\% selective
#' @param alpha "numeric" The capping parameter of the selectivity, to move the max away from one
#' @return "vector<numeric>" selectivity values over the age/length range and parameters supplied
#' @export
#'
Logistic = function(age_or_length,alpha = 1,a50,ato95) {
 selectivity = vector();
 for (i in 1:length(age_or_length)) {
  threshold = (a50 - age_or_length[i]) / ato95;
  if (threshold > 5) {
   selectivity[i] = 0.0;
  } else if (threshold < -5){
   selectivity[i] = alpha
  } else {
   selectivity[i] = alpha / (1.0 + pow(19.0, threshold));
  }  
 }
 return(selectivity);
}
