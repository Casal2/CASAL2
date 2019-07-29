#' @title check_mpd_identifiability
#'
#' @description
#' Do an eigen decomposition of the hessian matrix to identify correlated estimated parameters 
#' and parameters with high uncertainty. Small or zero eigenvalues (high condition number) 
#' indicates ill-posedness, i.e. the parameter estimation problem does not have a unique solution. 
#' This eigenvalue decomposition has been widely used in the estimation literature
#'
#' @author C.Marsh
#' @param model <casal2MPD> object that are generated from the extract.mpd() function. It expects the report of type
#' covariance_matrix to be present.
#' @return a data frame or message about the model
#' @rdname check_mpd_identifiability
#' @export check_mpd_identifiability
check_mpd_identifiability = function(cas2_mod, delta = .Machine$double.eps) {
  # check that there is covariance and estimate_valye report in this output
  covar_ndx = 0
  est_ndx = 0
  for(i in 1:length(cas2_mod)) {
    if (cas2_mod[[i]]$`1`$type == "covariance_matrix")
      covar_ndx = i
    if (cas2_mod[[i]]$`1`$type == "estimate_value")
      est_ndx = i
  }
  if (covar_ndx == 0) {
    stop("couldn't find a report of type 'covariance_matrix', please re-run the estimation with this report so we can check parameter identifiability")
  }
  ## check covariance is invertable
  if (!isSymmetric(cas2_mod[[covar_ndx]]$`1`$covariance_matrix))
    stop("covariance matrix is not symetric something is wrong here.")
  ## check positive semi defintie matrix
  if(class(try(solve(cas2_mod[[covar_ndx]]$`1`$covariance_matrix),silent=T)) != "matrix")
    stop("covariance not invertible")
  ## calculate hessian
  hess = solve(cas2_mod[[covar_ndx]]$`1`$covariance_matrix)
  ## eigen decomposition
  Eig = eigen(hess)
  WhichBad = which(Eig$values < sqrt(delta))
  df = NULL;
  if (length(WhichBad) == 0) {
    if (est_ndx != 0) {
      params = cas2_mod[[est_ndx]]$`1`$values
      df = data.frame(Param = names(params), 
                      MPD = as.numeric(params), 
                      Param_check =  "OK")
    } else {
      df = data.frame(Paramupdate. = 1:ncol(hess), 
                      MPD = NA, 
                      Param_check = "OK")
    }  
  } else {
    # for values with zero eigenvalues find the absolute largest eigenvector value
    RowMax = apply(Eig$vectors[, WhichBad, drop=FALSE], MARGIN = 1, 
                   FUN = function(vec) {max(abs(vec))})
    if (est_ndx != 0) {
      params = cas2_mod[[est_ndx]]$`1`$values
      df = data.frame(Param = names(params), 
                      MPD = as.numeric(params), 
                      Param_check = ifelse(RowMax > 0.1, "Bad", "OK"))
    } else {
      df = data.frame(Paramupdate. = 1:ncol(hess), 
                      MPD = NA, 
                      Param_check = ifelse(RowMax > 0.1, "Bad", "OK"))
    }
    
    
  }
  return(df)
}
