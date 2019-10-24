"extract.profiles" <-
function(filename)
{
# Extracts data from a casal -p run
# Outputs a list with one component for each parameter that is profiled
# Each component is a list with components
# values = n x 2 matrix with columns Value (parameter value) and
#          Objective (objective function at that value)
#          for n points in profile
# header = string containing description of parameter vector
# parameters = n x npar matrix contining parameter values at each profile point
# R.I.C.C. Francis
  string.match <- function(x, y) return((1:length(y))[regexpr(x, y, F) > 0])
  profile.list <- list()
  if((!missing(path) | path!="") & (substring(path,nchar(path)-1)!="\\" || substring(path,nchar(path)-1)!="/")) path<-paste(path,"\\",sep="")
  file <- casal.convert.to.lines(paste(path,filename,sep=""))
  name.lines <- string.match("Profile of parameter", file)
  parameter.lines <- string.match("Parameter estimates for profile of parameter", file)
  n.profiles <- length(name.lines)
  header.split <- casal.string.to.vector.of.words(file[parameter.lines[1] + 1])
  colnames <- c()
  k <- 1
  while(k < length(header.split)) {
    if(casal.is.whole.number(header.split[k + 1])) {
      n.reps <- as.numeric(header.split[k + 1])
      colnames <- c(colnames, paste(rep(header.split[k], n.reps), 1:n.reps, sep = "."))
      k <- k + 2
    }
    else {
      colnames <- c(colnames, header.split[k])
      k <- k + 1
    }
  }
  if(k == length(header.split))
    colnames <- c(colnames, header.split[length(header.split)])
  for(i in 1:n.profiles) {
    profile <- list()
    par.name <- casal.string.to.vector.of.words(file[name.lines[i]])[4]
    par.name <- substring(par.name, 1, nchar(par.name) - 1)
    subfile <- file[(name.lines[i] + 2):(parameter.lines[i] - 1)]
    n.values <- length(subfile)
    profile$values <- matrix(0, n.values, 2, dimnames = list(NULL, c("Value", "Objective")))
    for(j in 1:n.values)
      profile$values[j,  ] <- casal.string.to.vector.of.numbers(subfile[j])
    profile$header <- paste(header.split, collapse = " ")
    profile$parameters <- matrix(0, n.values, length(colnames), dimnames = list(NULL, colnames))
    subfile <- file[(parameter.lines[i] + 2):(parameter.lines[i] + n.values + 1)]
    for(j in 1:n.values)
      profile$parameters[j,  ] <- casal.string.to.vector.of.numbers(subfile[j])
    profile.list[[par.name]] <- profile
  }
  return(profile.list)
}
