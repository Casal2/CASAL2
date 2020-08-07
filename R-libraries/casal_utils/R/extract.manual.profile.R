"extract.manual.profile" <-
function(parnam, parindx, outfiles, dir)
{
# Extracts data from a manual profile on a parameter and creates an object
# like one component of the list created by functions extract.profiles and
# add.profile.objective.functions.  That is, a list with components
# values = n x 2 matrix with columns Value (parameter value) and
#          Objective (objective function at that value)
#          for n points in profile
# header = string containing description of parameter vector
# parameters = n x npar matrix contining parameter values at each profile point
# objfn a matrix in which rows correspond to components of the objective 
#         function and there is one column per point in the profile
#
# parnam, parindx: identify the parameter being profiled, e.g., if 
#       parnam='selectivity[fishery].all' and parindx=2 then the parameter 
#       being profiled is the 2nd parameter in the named selectivity 
# outfiles - vector of names of output files making up the manual profile.  
#            Each of these files should be the output from a 'casal -e' run
#            in which the profiled parameter has been fixed at some value.
# dir - directory in which these output files sit
#
	unlapply <- function(lst, fun, ...)
	unlist(lapply(lst, fun, ...))
	profile <- list()
	fitlist <- list()
	for(i in 1:length(outfiles))
		fitlist[[i]] <- input.mpd(paste(dir, outfiles[i], sep = ""))
	parval <- unlapply(fitlist, function(x, pn, pi)
	x$free[[pn]][pi], parnam, parindx)
	ord <- order(parval)
	fitlist <- fitlist[ord]
	parval <- parval[ord]
	profile$values <- cbind(Value = parval, Objective = unlapply(fitlist, 
		function(x)
	x$obj$value))
	pardet <- list(names = names(fitlist[[1]]$free), length = unlapply(
		fitlist[[1]]$free, length))
	temp <- paste(pardet$names, ifelse(pardet$length > 1, paste("", pardet$
		length), ""), sep = "")
	profile$header <- paste(temp, collapse = " ")
	profile$header
	temp <- rep("", sum(pardet$length))
	indx <- 1
	for(i in 1:length(pardet$names)) {
		if(pardet$length[i] > 1)
			temp[indx:(indx + pardet$length[i] - 1)] <- paste(1:
				pardet$length[i])
		indx <- indx + pardet$length[i]
	}
	colnam <- paste(rep(pardet$names, pardet$length), rep(ifelse(pardet$
		length == 1, "", "."), pardet$length), temp, sep = "")
	profile$parameters <- matrix(unlapply(fitlist, function(x)
	unlist(x$free)), nrow(profile$values), length(colnam), byrow = T, 
		dimnames = list(paste(parval), colnam))
	rownam <- fitlist[[1]]$obj$comp[, 1]
	profile$objfn <- matrix(unlapply(fitlist, function(x)
	x$obj$comp[, 2]), length(rownam), length(parval), dimnames = list(
		rownam, paste(parval)))
	profile$objfn
	profile
}

