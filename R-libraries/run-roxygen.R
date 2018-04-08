if (!is.element('roxygen2', installed.packages()[,1])) {
  cat("installing the R library 'roxygen2', it could not be found in your R packages")
  install.packages('roxygen2', repos = "https://cloud.r-project.org")
}
  
require(roxygen2)
roxygen2::roxygenize("CASAL2/")

