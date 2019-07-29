## R script for calculating Residuals.

## Utilty functions
Paste = function(..., sep = ""){paste(..., sep = sep)}
robustify = function(X,r = 0.0001) {
  robust = vector();
  for(i in 1:length(X)) {
    if (X[i] >= r ) {
      robust[i]= X[i]
    } else {
      robust[i]= (r / (2 - X[i]/r))
    }
  }
  return(robust)
}

CalcPearsonsResiduals = function(Observation,likelihood = "multinomial",tolerance = 0.0001) {
  if (!likelihood %in% c("multinomial","binomial","normal","lognormal","robustified-lognormal","normal-log","binomial-approx","normal-by-std-dev"))
    stop(Paste("the likelihood you inputed was ", likelihood, " it must be one of the following multinomial, binomial, normal, lognormal, robustified-lognormal, normal-log, binomial-approx, normal-by-std-dev"))
  ## decide what level you want the user to define parameters.
  Cols = colnames(Observation);
  if (!all(Cols %in% c("year", "category", "age", "length", "observed","expected","residual","error_value","score")))
    stop("Check the observation has the following colnames: year, category, age, length, observed, expected, residual, error_value, score")
  
  switch(likelihood,
    "multinomial" = sqrt((robustify(Observation[,"expected"], tolerance) * (1 - robustify(Observation[,"expected"], tolerance))) / Observation[,"error_value"]),
    "binomial-approx" = sqrt(((Observation[,"expected"] + tolerance) * (1 - Observation[,"expected"] + tolerance)) / Observation[,"error_value"]),
    "binomial" = sqrt((robustify(Observation[,"expected"], tolerance) * (1 - robustify(Observation[,"expected"], tolerance))) / Observation[,"error_value"]),
    "lognormal" = Observation[,"expected"] * Observation[,"error_value"],
    "normal-log" = Observation[,"expected"] * Observation[,"error_value"],
    "normal" = Observation[,"expected"] * Observation[,"error_value"],
    "robustified-lognormal" = Observation[,"expected"] * Observation[,"error_value"],
    "normal-by-std-dev" = Observation[,"error_value"]
    )
}


CalcNormResiduals = function(Observation,likelihood = "multinomial",tolerance = 0.0001) {
  if (!likelihood %in% c("normal","lognormal","robustified-lognormal","normal-log"))
    stop(Paste("the likelihood you inputed was ", likelihood, " it must be one of the following normal, lognormal, robustified-lognormal, normal-log"))
  ## decide what level you want the user to define parameters.
  Cols = colnames(Observation);
  if (!all(Cols %in% c("year", "category", "age", "length", "observed","expected","residual","error_value","score")))
    stop("Check the observation has the following colnames: year, category, age, length, observed, expected, residual, error_value, score")
  
  switch(likelihood,
    "lognormal" = (log(Observation[,"observed"] / Observation[,"expected"]) + 0.5 * sqrt(log(1 + Observation[,"error_value"] ^ 2))) / sqrt(log(1 + Observation[,"error_value"] ^ 2))),
    "normal-log" = log(Observation[,"observed"] / Observation[,"expected"]) / sqrt(log(1 + Observation[,"error_value"] ^ 2)),
    "normal" = Observation[,"expected"] * Observation[,"error_value"],
    "robustified-lognormal" = (log(Observation[,"observed"] / Observation[,"expected"]) + 0.5 * sqrt(log(1 + Observation[,"error_value"] ^ 2))) / sqrt(log(1 + Observation[,"error_value"] ^ 2))),
    )
}


## Debug 
CalcPearsonsResiduals(MPD$tan_at_age$'1'$Comparisons, likelihood = "multinomial")

CalcNormResiduals(MPD$tan_at_age$'1'$Comparisons, likelihood = "multinomial")
CalcNormResiduals(MPD$tan_at_age$'1'$Comparisons, likelihood = "multinomial")

