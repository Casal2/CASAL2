"add.profile.objective.functions" <-
function(profile.list, filename)
{
# Reads multiple objective functions from file filename and inserts them
# in the appropriate places in object profile.list
#
# profile.list should be a list of profiles (as output by extract.profiles)
# filename should be the result from "casal -i profpars -r > filename"
# where file profpars was produced by Wrprof.pars(profile.list,'profpars')
#
# Outputs object profile.list with an additional component, objfn, added
# to each profile, where objfn is a matrix in which rows correspond to
# components of the objective function and there is one column per
# point in the profile
#
	objfnmat <- extract.multiple.objective.functions(filename)
	n.points.per.profile <- unlist(lapply(profile.list, function(x)
	nrow(x$values)))
	if(ncol(objfnmat) != sum(n.points.per.profile))
		stop("Wrong number of objective functions")
	i <- 1
	for(j in 1:length(profile.list)) {
		profile.list[[j]]$objfn <- objfnmat[, i:(i + 
			n.points.per.profile[j] - 1)]
		dimnames(profile.list[[j]]$objfn)[[2]] <- dimnames(profile.list[[
			j]]$values)[[1]]
		i <- i + n.points.per.profile[j]
	}
	profile.list
}

