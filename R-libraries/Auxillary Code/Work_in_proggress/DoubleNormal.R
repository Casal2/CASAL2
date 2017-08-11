#' Calculate the Double-Normal Selectivity used in Casal2
#'
#' This function calculates the Double-Normal selectivity and should be used to plot up the form of the selectivity when estimating the parameters in the model.
#'
#' @author Craig Marsh
#' @param age_or_length "vector<numeric>" A vector of ages and or lengths to calculate the selectivity over.
#' @param mu "numeric" Mean of the selectivity
#' @param sigma_l "numeric" left hand standard deviation of the selectivity
#' @param sigma_r "numeric" Right hand standard deviation of the selectivity
#' @param alpha "numeric" The capping parameter of the selectivity, to move the max away from one
#' @return "vector<numeric>" selectivity values over the age/length range and parameters supplied
#' @export
#'
DoubleNormal = function(age_or_length,alpha = 1,mu,sigma_l,sigma_r) {
  selectivity = vector();
  for(i in 1:length(age_or_length)) {
    if(age_or_length[i] < mu) {
      selectivity[i] = pow(2.0, -((age_or_length[i] - mu) / sigma_l * (age_or_length[i] - mu) / sigma_l)) * alpha;
    } else {
      selectivity[i] = pow(2.0, -((age_or_length[i] - mu) / sigma_r * (age_or_length[i] - mu) / sigma_r)) * alpha;    
    }   
  }
  return(selectivity);
}


