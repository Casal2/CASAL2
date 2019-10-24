"extend.profile" <-
function(profile, parnam, parindx, outfiles, dir)
{
# Extends an existing profile on a parameter by adding information from
# additional manual profile runs.
#
# profile = an object containing a profile (like that created by 
#           extract.manual.profile)
# parnam, parindx: identify the parameter being profiled, e.g., if 
#       parnam='selectivity[fishery].all' and parindx=2 then the parameter 
#       being profiled is the 2nd parameter in the named selectivity 
# outfiles - vector of names of output files from the the manual profile runs.
#            Each of these files should be the output from a 'casal -e' run
#            in which the profiled parameter has been fixed at some value.
# dir - directory in which these output files sit
# 
	unlapply <- function(lst, fun, ...)
	unlist(lapply(lst, fun, ...))
	addprof <- extract.manual.profile(parnam, parindx, outfiles, dir)
	parval <- c(profile$values[, "Value"], addprof$values[, "Value"])
	ord <- order(parval)
	parval <- parval[ord]
	profile$values <- rbind(profile$values, addprof$values)[ord,  ]
	profile$parameters <- rbind(profile$parameters, addprof$parameters)[ord,
		]
	if(!is.na(match("objfn", names(profile))))
		profile$objfn <- cbind(profile$objfn, addprof$objfn)[, ord]
	profile
}

