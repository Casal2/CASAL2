# 
# import Casal2 functions
funs = list.files(file.path("..","casal2","R"))
for(i in 1:length(funs))
  source(file.path("..","casal2","R",funs[i]))

## load Chis Francis's Dataweighting package.
packages_required = c("DataWeighting")
for(i in 1:length(packages_required)) {
  print(i)
  if(packages_required[i] %in% rownames(installed.packages()) == FALSE) {
    cat("installing package ", packages_required[i], "\n")
    install.packages(file.path("..","DataWeighting_1.0.zip"),repos = NULL)
  } else {
    library(packages_required[i],character.only=TRUE)
  }
}



