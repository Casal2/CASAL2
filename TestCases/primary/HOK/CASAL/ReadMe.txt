### Hoki Run 1.7 (version) ###

# Original supplied by Andy McKenzie, from the 2018 assessment.

# A summary of the model assumptions #

Age based.
Partitioned by age (17 with plus group), stock (2), area (4). Not partitioned by sex or maturity.
Five annual time steps
Not using the Baranov. 
Six fisheries. 
Five migrations. 
YCS estimated, assuming the BH. 
M constant at age. 
Umax assumed. 
Ogives used are constant, age-based double normal, age-based double normal shifted, length-based logistic, length-based double normal 	# final two added by MD for this test case
Growth is VBGF. 
B0 estimated as log_B0_total, with @B0_propr_stock1 	# This is an unusual case, and the only assessment I know where this is used

Ageing error using misclassification matrix
free q's (some with lognormal informed priors)
Abundance index process error estimated
@B0_propr_stock1 with beta prior  	# Im not sure the beta prior is documented in the casal manual?
Selectivity shift parameter, with normal-by-stdev prior
Other priors uniform. 
M estimated, with lognormal prior.

Observations: 
@relative_abundance (lognormal)
@catch_at (multinomial)
@proportions_at (multinomial)
@proportions_migrating

The age observations have been thinned to only those needed (3 sets removed from original assessment; two OLF sets and autumn SA). 
The eastern spawning selectivity ogive has been made constant (previously double normal; confounded to some degree with migration).
The autumn SA survey has been removed. 

Penalties:
YCS_difference
catch_limit (Umax)
ogive_difference (migrations)

phased estimation MAY be on 	# some specified as phase 1, others no phase specified - is there some default behaviour?
				# we might turn this off, if we're not sure? 
				# Or introduce phasing throughout? If we need to test phasing, this would be a suitable candidate.

# Convergence successful. But slow; 0.34 hours.    




