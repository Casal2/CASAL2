if (!is.element('roxygen2', installed.packages()[,1])) {
  cat("Installing the R package 'roxygen2', as it was not found in the installed R packages")
  install.packages('roxygen2', repos = "https://cloud.r-project.org")
}

require(roxygen2)
roxygen2::roxygenize(paste0(getwd(),"/casal2"), clean = TRUE)

