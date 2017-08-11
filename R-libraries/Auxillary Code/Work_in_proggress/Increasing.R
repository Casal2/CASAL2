#' Calculate the Increasing Selectivity used in Casal2
#'
#' This function calculates the Increasing selectivity and should be used to plot up the form of the selectivity when estimating the parameters in the model.
#'
#' @author Craig Marsh
#' @param age_or_length "vector<numeric>" A vector of ages and or lengths to calculate the selectivity over.
#' @param v "vector<numeric>" A vector of selectivities, must have a one for one relationship with age_or_length.
#' @param low "numeric" age or length at which selectivity is set = 0 below
#' @param high "numeric" age or length at which selectivity is set = alpha above
#' @param alpha "numeric" The capping parameter of the selectivity, to move the max away from one
#' @return "vector<numeric>" selectivity values over the age/length range and parameters supplied
#' @export
#'
Increasing = function(age_or_length, alpha = 1,v,low,high) {
 if(length(age_or_length) != length(v)) 
  stop("This function will only work if v is the same length as the age_or_length vector")
 
 for(i in 1:length(age_or_length)) {
  if(age_or_length[i] < low) {
   selectivity[i] = 0.0;
  } else if (age_or_length[i] > high){
   selectivity[i] = v[length(v)] * alpha;
  } else {
   value = v[1];
   j = low + 1
   while(j < i) {
     if (j > high || value >= alpha)
      break;
     value = value + (alpha - value) * v[j - low]; ## might need to check this
   }
   selectivity[i] = value;
  }
 }
 return(selectivity);
}