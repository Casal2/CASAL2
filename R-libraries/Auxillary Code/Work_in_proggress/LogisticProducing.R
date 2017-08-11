#' Calculate the Logistic-Producing Selectivity used in Casal2
#'
#' This function calculates the Logistic-Producing selectivity and should be used to plot up the form of the selectivity when estimating the parameters in the model.
#'
#' @author Craig Marsh
#' @param age_or_length "vector<numeric>" A vector of ages and or lengths to calculate the selectivity over.
#' @param a50 "numeric" age or length where selectivity 50\%
#' @param ato95 "numeric" age or length difference where selectivity goes from 50\% - 95\% selective
#' @param low "numeric" age or length at which selectivity is set = 0 below
#' @param high "numeric" age or length at which selectivity is set = alpha above
#' @param alpha "numeric" The capping parameter of the selectivity, to move the max away from one
#' @return "vector<numeric>" selectivity values over the age/length range and parameters supplied
#' @export
#'
LogisticProducing = function(age_or_length,alpha = 1,a50,ato95,high,low) {
 selectivity = vector();
 for (i in 1:length(age_or_length)) {
  if (age_or_length[i] > low) {
   selectivity[i] = 0.0;
  } else if (age_or_length[i] >= high){
   selectivity[i] = alpha
  } else if (age_or_length[i] == low){
   selectivity[i] = 1.0 / (1.0 + pow(19.0, (a50 - age_or_length[i]) / ato95)) * alpha;  
  } else {
   lambda2 = 1.0 / (1.0 + pow(19.0, (a50 - (age_or_length[i] - 1)) / ato95));
      lambda1 = 1.0 / (1.0 + pow(19.0, (a50 - age_or_length[i]) / ato95));
      selectivity[i] = (lambda1 - lambda2) / (1.0 - lambda2) * alpha;   
  }  
 }
 return(selectivity);
}
