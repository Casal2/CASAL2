#' Utility extract function
#'
#' @author Craig marsh
#'
"transform.comparisons" = function(Data) {
  years = unique(Data[,"year"])
  n_categories = length(unique(Data[,"category"]))
  transformed_data = list();

  if (n_categories > 2) {
    stop(Paste("This function can currently only deal with an age composition with 2 unique categories, but we found ", n_categories, " plz check you have selected the correct observation to weight"))
  }
  ages = unique(Data[,"age"])
  lengths = unique(Data[,"length"])
  if (length(ages) > 1 & length(lengths) > 1) {
    stop("This observation contains both length and age bins. something is wrong.")
  }
  bins = 0
    if (length(ages) > 1) {
    bins = ages
  } else {
    bins = lengths;
  }
  n_bins = length(bins)  
  n_years = length(years)

  obs = matrix(Data[, "observed"], byrow = T, ncol = n_bins * n_categories, nrow = n_years)
  fit = matrix(Data[, "expected"], byrow = T, ncol = n_bins * n_categories, nrow = n_years)
  err = matrix(Data[, "error_value"], byrow = T, ncol = n_bins * n_categories, nrow = n_years)
  adjusted_error = matrix(Data[, "adjusted_error"], byrow = T, ncol = n_bins * n_categories, nrow = n_years)
  if ("normalised_residuals" %in% colnames(Data))
    norm_resid = matrix(Data[, "normalised_residuals"], byrow = T, ncol = n_bins * n_categories, nrow = n_years)
  if ("pearsons_residuals" %in% colnames(Data))    
    pear_resid = matrix(Data[, "pearsons_residuals"], byrow = T, ncol = n_bins * n_categories, nrow = n_years)
  
  rownames(fit) = rownames(err) = rownames(obs) = years
  if(n_categories == 1) {
    if (n_bins == 1) {
      colnames(obs) = unique(Data[,"category"])
      colnames(fit) = unique(Data[,"category"])
      colnames(err) = unique(Data[,"category"])   
      colnames(adjusted_error) = unique(Data[,"category"])  
      if ("normalised_residuals" %in% colnames(Data))
        colnames(norm_resid) = unique(Data[,"category"])        
      if ("pearsons_residuals" %in% colnames(Data))    
        colnames(pear_resid) = unique(Data[,"category"])  
    } else {
      colnames(obs) = Paste(unique(Data[,"category"]), "[",bins,"]")
      colnames(fit) = Paste(unique(Data[,"category"]), "[",bins,"]")
      colnames(err) = Paste(unique(Data[,"category"]), "[",bins,"]")   
      colnames(adjusted_error) = Paste(unique(Data[,"category"]), "[",bins,"]")        
      if ("normalised_residuals" %in% colnames(Data))
        colnames(norm_resid) = Paste(unique(Data[,"category"]), "[",bins,"]")     
      if ("pearsons_residuals" %in% colnames(Data))    
        colnames(pear_resid) = Paste(unique(Data[,"category"]), "[",bins,"]")        
    }
  } else {
    if (n_bins == 1) {
      colnames(obs) = c(unique(Data[,"category"])[1],unique(Data[,"category"])[2])
      colnames(fit) = c(unique(Data[,"category"])[1],unique(Data[,"category"])[2])
      colnames(err) = c(unique(Data[,"category"])[1],unique(Data[,"category"])[2]) 
      colnames(adjusted_error) = c(unique(Data[,"category"])[1],unique(Data[,"category"])[2])          
      if ("normalised_residuals" %in% colnames(Data))
        colnames(norm_resid) = c(unique(Data[,"category"])[1],unique(Data[,"category"])[2])    
      if ("pearsons_residuals" %in% colnames(Data))    
        colnames(pear_resid) = c(unique(Data[,"category"])[1],unique(Data[,"category"])[2])          
    } else {
      colnames(obs) = c(Paste(unique(Data[,"category"])[1], "[",bins,"]"),Paste(unique(Data[,"category"])[2], "[",bins,"]"))
      colnames(fit) = c(Paste(unique(Data[,"category"])[1], "[",bins,"]"),Paste(unique(Data[,"category"])[2], "[",bins,"]"))
      colnames(err) = c(Paste(unique(Data[,"category"])[1], "[",bins,"]"),Paste(unique(Data[,"category"])[2], "[",bins,"]"))      
      colnames(adjusted_error) = c(Paste(unique(Data[,"category"])[1], "[",bins,"]"),Paste(unique(Data[,"category"])[2], "[",bins,"]"))         
      if ("normalised_residuals" %in% colnames(Data))
        colnames(norm_resid) = c(Paste(unique(Data[,"category"])[1], "[",bins,"]"),Paste(unique(Data[,"category"])[2], "[",bins,"]"))    
      if ("pearsons_residuals" %in% colnames(Data))    
        colnames(pear_resid) = c(Paste(unique(Data[,"category"])[1], "[",bins,"]"),Paste(unique(Data[,"category"])[2], "[",bins,"]"))             
    }
  }
    transformed_data$obs = obs
    transformed_data$fits = fit
    transformed_data$error.value = err
    transformed_data$actual_error = adjusted_error    
    transformed_data$year = years   
    if ("normalised_residuals" %in% colnames(Data))
      transformed_data$norm_resid = norm_resid
    if ("pearsons_residuals" %in% colnames(Data))    
      transformed_data$pear_resid = pear_resid

  return(transformed_data)
}